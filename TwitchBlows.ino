// ─────────────────────────────────────────────
//  TwitchBlows — ESP32-C3  +  2x SN74HC595 (16 outputs)
// ─────────────────────────────────────────────
#define DEBUG 1          // Set 0 to silence all serial output

#if DEBUG
  #define DPRINT(x)   Serial.print(x)
  #define DPRINTLN(x) Serial.println(x)
#else
  #define DPRINT(x)
  #define DPRINTLN(x)
#endif

// ── Includes ──────────────────────────────────
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <Secrets.h>
#include <time.h>
#include "html.h"

// ── Configuration ─────────────────────────────
#define HOSTNAME      "twitchblows"
#define WIFI_TIMEOUT  5000      // ms to wait for STA connection
#define AP_SSID       HOSTNAME   // Captive portal AP name

// SN74HC595 pins (adjust to your wiring)
#define PIN_DATA      4    // DS   (SER)   → 595 pin 14
#define PIN_CLOCK     6    // SRCLK        → 595 pin 11
#define PIN_LATCH     5    // RCLK (ST_CP) → 595 pin 12
#define PIN_OE        3    // OE  (active-low) → 595 pin 13
//           595 pin 10 (SRCLR)→ VCC  (active-low clear, keep high)
//           595 VCC            → 3.3V (match ESP32-C3 logic levels)

#define PIN_CURRENT   2

// Current sensor calibration — ACS-style, midpoint ~1.65V on 3.3V/12-bit ADC
#define CS_MIDPOINT_V     2.5f   // V at zero current (tune to your sensor's actual idle reading)
#define CS_MV_PER_AMP    -100.0f   // mV/A sensitivity (e.g. ACS712-5A=185, 20A=100, 30A=66)
#define CS_DETECT_AMPS  5.0f    // minimum current (A) to count as live output

// ── Globals ───────────────────────────────────
WebServer   server(80);
DNSServer   dns;
Preferences prefs;

bool    apMode  = false;
int8_t   activeQ = -1;   // -1 = all off, 0-15 = active output (toggle)

// Pulse state
bool     pulseActive = false;
int8_t   pulseQ      = -1;
uint32_t pulseEnd    = 0;

// Output tracking
uint16_t usedOutputs = 0;       // bitmask: bit N=1 means output N is dead/used
int      nextOutput  = 0;       // rolling pointer for next untried output
int      channelPeak[16] = {0}; // last measured ADC peak per channel
int8_t  pendingUsedQ = -1;     // channel to mark used when pulse ends

// Twitch IRC
WiFiClientSecure twitchClient;
bool              twitchConnected = false;
unsigned long     lastTwitchPing = 0;

// Twitch trigger config
int     bitsThreshold = 100;    // bits needed to trigger one output
int     pointsThreshold = 1;     // min channel points redemptions to trigger
int     subsThreshold = 1;       // min subs to trigger
int     raidThreshold = 5;       // min raid viewer count to trigger
uint32_t pulseDurMs   = 500;     // how long each output fires (ms)
String  twitchChannel = "daverdavid";  // loaded from prefs

// Event counters
int pointsRedemptionCount = 0;
int subCount = 0;

// Current sense config
bool     sensorReady = false;   // set true after boot check passes
uint32_t currentSenseDelayMs = 10;  // ms to wait after firing before reading ADC
int     adcMax = 0;            // max ADC reading in last 5 seconds (mV)
uint32_t adcMin = 0xFFFFFFFF; // min ADC reading in last 5 seconds (mV)
uint32_t adcMaxTime = 0;      // timestamp when adcMax was recorded
uint32_t adcMinTime = 0;    // timestamp when adcMin was recorded
float   ampMax = 0.0f;       // max amperage in last 5 seconds
float   ampMin = 0.0f;        // min amperage in last 5 seconds
uint32_t ampMaxTime = 0;     // timestamp when ampMax was recorded
uint32_t ampMinTime = 0;    // timestamp when ampMin was recorded

// Event enable flags
bool evBitsEnabled   = true;
bool evPointsEnabled = true;
bool evSubsEnabled   = false;
bool evRaidsEnabled  = false;

// ── Fire queue + rate limiting ─────────────────
#define FIRE_QUEUE_SIZE 16
int8_t  fireQueue[FIRE_QUEUE_SIZE];
int     fireQueueHead = 0;
int     fireQueueTail = 0;

uint32_t minGapMs     = 2000;   // min ms between fires (configurable)
uint32_t lastFireTime = 0;      // timestamp of last fire

// Channel points reward ID filter (empty = trigger on ALL redemptions)
String pointsRewardFilter = "";

// ── Web console log buffer ─────────────────────
#define LOG_LINES 120
#define LOG_WIDTH 160
char logBuf[LOG_LINES][LOG_WIDTH];
int  logHead = 0;
int  logCount = 0;

void webLog(const String& msg) {
  struct tm ti;
  char ts[22];
  if (getLocalTime(&ti, 0)) {
    snprintf(ts, sizeof(ts), "[%02d:%02d:%02d] ", ti.tm_hour, ti.tm_min, ti.tm_sec);
  } else {
    uint32_t s = millis() / 1000;
    snprintf(ts, sizeof(ts), "[+%lus] ", s);
  }
  String stamped = String(ts) + msg;
  DPRINTLN(stamped);
  stamped.toCharArray(logBuf[logHead], LOG_WIDTH - 1);
  logBuf[logHead][LOG_WIDTH - 1] = '\0';
  logHead = (logHead + 1) % LOG_LINES;
  if (logCount < LOG_LINES) logCount++;
}

void handleLog() {
  String out = "";
  int start = (logCount < LOG_LINES) ? 0 : logHead;
  for (int i = 0; i < min(logCount, LOG_LINES); i++) {
    int idx = (start + i) % LOG_LINES;
    out += String(logBuf[idx]) + "\n";
  }
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/plain", out);
}

// ── 595 helper ────────────────────────────────
void shiftWrite(uint16_t val) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, (val >> 8) & 0xFF);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, val & 0xFF);
  digitalWrite(PIN_LATCH, HIGH);
}

void shiftWriteEnabled(uint16_t val) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, (val >> 8) & 0xFF);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, val & 0xFF);
  digitalWrite(PIN_LATCH, HIGH);
  if (val != 0) digitalWrite(PIN_OE, LOW);
}

void disableOutputs() {
  digitalWrite(PIN_OE, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0x00);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0x00);
  digitalWrite(PIN_LATCH, HIGH);
}

float adcToAmps(int millivolts) {
  float v = millivolts / 1000.0f;
  return (v - CS_MIDPOINT_V) / (CS_MV_PER_AMP / 1000.0f);
}

// ── Safe pulse: guaranteed single-output, always-off-after ─
void safePulse(int8_t q, uint32_t ms) {
  disableOutputs();
  pulseActive = false;
  pulseQ      = -1;
  activeQ     = -1;

  if (q < 0 || q > 15) return;

  shiftWriteEnabled((uint16_t)(1u << q));
  pulseActive = true;
  pulseQ      = q;
  pulseEnd    = millis() + ms;
  webLog("[FIRE] Ch" + String(q+1) + " ON for " + String(ms) + "ms");
}

// ── Fire next live output with current sensing ─
int fireNextOutput(uint32_t pulseDurationMs) {
  if (!sensorReady) { webLog("[FIRE] Blocked — sensor not ready"); return -1; }

  disableOutputs();

  for (int tries = 0; tries < 16; tries++) {
    int q = nextOutput;
    nextOutput = (nextOutput + 1) % 16;

    if (usedOutputs & (uint16_t)(1u << q)) {
      webLog("[FIRE] Ch" + String(q+1) + " already used — skip");
      continue;
    }

    shiftWriteEnabled((uint16_t)(1u << q));

    // Sample peak deviation from midpoint over sense window
    float peakAmps = 0.0f;
    uint32_t senseStart = millis();
    while (millis() - senseStart < currentSenseDelayMs) {
      int mv = analogReadMilliVolts(PIN_CURRENT);
      float a = fabsf(adcToAmps(mv));
      if (a > peakAmps) peakAmps = a;
    }
    channelPeak[q] = (int)(peakAmps * 1000.0f);  // store as mA

    // Deviation check: voltage must move >0.35V from midpoint
    float csThreshAmps = CS_DETECT_AMPS;
    if (peakAmps < csThreshAmps) {
      disableOutputs();
      usedOutputs |= (uint16_t)(1u << q);
      webLog("[Ch" + String(q+1) + "] peak=" + String(peakAmps,3) + "A — DEAD");
      continue;
    }

    // Use safePulse for guaranteed single-output, always-off-after
    safePulse(q, pulseDurationMs);
    pendingUsedQ = q;
    webLog("[Ch" + String(q+1) + "] peak=" + String(peakAmps,3) + "A — FIRING " + String(pulseDurationMs) + "ms");
    return q;
  }

  webLog("[FIRE] All 16 outputs exhausted");
  disableOutputs();
  return -1;
}

void queueFire() {
  int next = (fireQueueTail + 1) % FIRE_QUEUE_SIZE;
  if (next == fireQueueHead) {
    webLog("[QUEUE] Full — trigger dropped");
    return;
  }
  fireQueue[fireQueueTail] = 1;  // value is a token; just enqueue a 1
  fireQueueTail = next;
  webLog("[QUEUE] Trigger enqueued (" + String((fireQueueTail - fireQueueHead + FIRE_QUEUE_SIZE) % FIRE_QUEUE_SIZE) + " pending)");
}

// ── JSON response helper ────────────────────────
// Sends JSON with keep-alive so the browser reuses the TCP connection,
// eliminating per-request handshake overhead and cutting response latency.
void sendJSON(int code, const String &json) {
  server.sendHeader("Connection",                "keep-alive");
  server.sendHeader("Cache-Control",             "no-store");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(code, "application/json", json);
}

// ── WiFi ──────────────────────────────────────
bool connectWifi(const String &ssid, const String &psk) {
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.setHostname(HOSTNAME);

  // RF calibration kick
  WiFi.begin(ssid.c_str(), psk.c_str());
  delay(500);
  WiFi.disconnect(true);
  delay(200);

  // Retry loop (3 attempts, 5s each)
  for (int attempt = 0; attempt < 3; attempt++) {
    DPRINT("Connecting to "); DPRINT(ssid); DPRINT(" attempt "); DPRINTLN(attempt + 1);
    WiFi.begin(ssid.c_str(), psk.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
      delay(100); DPRINT(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      DPRINTLN("");
      return true;
    }
    WiFi.disconnect(true);
    delay(500);
  }
  DPRINTLN("failed");
  return false;
}

void startAP() {
  apMode = true;
  WiFi.mode(WIFI_AP);
  WiFi.setTxPower(WIFI_POWER_15dBm);
  WiFi.softAP(AP_SSID);
  dns.start(53, "*", WiFi.softAPIP());
  DPRINT("AP started: "); DPRINTLN(AP_SSID);
  DPRINT("AP IP: ");      DPRINTLN(WiFi.softAPIP());
}

// ── Web routes ────────────────────────────────
void handleRoot() {
  if (apMode) {
    server.send_P(200, "text/html", PORTAL_HTML);
  } else {
    server.send_P(200, "text/html", INDEX_HTML);
  }
}

void handleSet() {
  if (server.hasArg("q")) {
    int q = server.arg("q").toInt();
    if (q < 0 || q > 15) q = -1;
    if (q < 0) {
      disableOutputs();
    } else {
      safePulse((int8_t)q, 500);  // manual toggle uses 500ms default
    }
  }
  sendJSON(200, "{\"active\":" + String(activeQ) + "}");
}

// /pulse?q=N&ms=M  — raise output N for M ms then auto-drop in loop()
void handlePulse() {
  if (!server.hasArg("q") || !server.hasArg("ms")) {
    sendJSON(400, "{\"ok\":false,\"err\":\"missing q or ms\"}");
    return;
  }
  int q  = server.arg("q").toInt();
  int ms = constrain(server.arg("ms").toInt(), 10, 30000);
  if (q < 0 || q > 15) {
    sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}");
    return;
  }
  safePulse((int8_t)q, (uint32_t)ms);
  sendJSON(200, "{\"ok\":true,\"q\":" + String(q) + ",\"ms\":" + String(ms) + "}");
}

void handleState() {
  String json = "{\"active\":"    + String(activeQ) +
                ",\"pulse\":"     + (pulseActive ? "true" : "false") +
                ",\"pulseQ\":"   + String(pulseActive ? pulseQ : -1) +
                ",\"used\":"     + String(usedOutputs) +
                ",\"twitch\":"   + (twitchConnected ? "true" : "false") +
                ",\"bitsThresh\":" + String(bitsThreshold) +
                ",\"pulseDurMs\":" + String(pulseDurMs) +
                ",\"nextQ\":"   + String(nextOutput) +
                ",\"sensorOK\":" + String(sensorReady ? "true" : "false") +
                ",\"adcCurr\":" + String(sensorReady ? analogReadMilliVolts(PIN_CURRENT) : 0) +
                ",\"adcMax\":" + String(adcMax) +
                ",\"adcMin\":" + String(adcMin) +
                ",\"ampCurr\":" + String(sensorReady ? String(fabsf(adcToAmps(analogReadMilliVolts(PIN_CURRENT))), 3) : "0") +
                ",\"ampMax\":" + String(ampMax, 3) +
                ",\"ampMin\":" + String(ampMin, 3) +
                ",\"peaks\":[";
  for (int i = 0; i < 16; i++) {
    json += String(channelPeak[i]);
    if (i < 15) json += ",";
  }
  json += "]}";
  sendJSON(200, json);
}

void handleSaveWifi() {
  if (server.hasArg("ssid")) {
    String ssid = server.arg("ssid");
    String psk  = server.arg("psk");
    prefs.begin("wifi", false);
    prefs.putString("ssid", ssid);
    prefs.putString("psk",  psk);
    prefs.end();
    server.send(200, "text/html",
      "<html><body style='font-family:sans-serif;background:#0f1117;color:#e2e8f0;text-align:center;padding:2rem'>"
      "<h2 style='color:#e94560'>Saved!</h2><p>Rebooting to connect...</p></body></html>");
    delay(1500);
    ESP.restart();
  } else {
    server.sendHeader("Location", "/");
    server.send(302);
  }
}

void handleNotFound() {
  if (apMode) {
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302);
  } else {
    server.send(404, "text/plain", "not found");
  }
}

// ── Twitch IRC helpers ─────────────────────────
String extractIRCMessage(const String& line) {
  int cmdPos = line.indexOf(" PRIVMSG ");
  if (cmdPos < 0) cmdPos = line.indexOf(" USERNOTICE ");
  if (cmdPos < 0) return "";

  int hashPos = line.indexOf('#', cmdPos);
  if (hashPos < 0) return "";

  int spaceAfterChan = line.indexOf(' ', hashPos);
  if (spaceAfterChan < 0) return "";

  if (spaceAfterChan + 1 >= (int)line.length() || line[spaceAfterChan + 1] != ':') return "";

  String payload = line.substring(spaceAfterChan + 2);
  payload.replace("\r", "");
  payload.trim();
  return payload;
}

String extractTag(const String& line, const String& tagName) {
  String search = tagName + "=";
  int start = line.indexOf(search);
  if (start < 0) return "";
  start += search.length();
  int end = line.indexOf(';', start);
  int spaceEnd = line.indexOf(' ', start);
  if (end < 0 || (spaceEnd >= 0 && spaceEnd < end)) end = spaceEnd;
  if (end < 0) end = line.length();
  return line.substring(start, end);
}

void parseTwitchMessage(const String& msg) {
  String msgId   = extractTag(msg, "msg-id");
  String user    = extractTag(msg, "display-name");
  String bitsStr = extractTag(msg, "bits");
  String rewardId = extractTag(msg, "custom-reward-id");

  // Bot detection
  static const char* knownBots[] = {"Nightbot","StreamElements","Moobot","Fossabot","Streamlabs","CommanderRoot",nullptr};
  bool isBot = false;
  for (int i = 0; knownBots[i] != nullptr; i++) {
    if (user.equalsIgnoreCase(knownBots[i])) { isBot = true; break; }
  }
  if (isBot) { webLog("[IRC] (bot) " + user + " ignored"); return; }

  if (bitsStr.length() > 0 && bitsStr != "0") {
    webLog("[IRC] BITS event — user=" + user + " bits=" + bitsStr);
  }
  if (rewardId.length() > 0) {
    webLog("[IRC] CHANNEL POINTS — user=" + user + " reward-id=" + rewardId);
  }
  if (msgId == "sub" || msgId == "resub" || msgId == "subgift") {
    webLog("[IRC] SUB event — user=" + user + " msg-id=" + msgId);
  }
  if (msgId == "raid") {
    webLog("[IRC] RAID event — user=" + user);
  }
  if (bitsStr.length() == 0 && rewardId.length() == 0 && msgId.length() == 0) {
    webLog("[IRC] CHAT msg — user=" + user + " msg=" + extractIRCMessage(msg));
  }

  // Trigger logic
  if (evBitsEnabled && bitsStr.length() > 0) {
    int bitsCount = bitsStr.toInt();
    if (bitsCount > 0 && bitsCount >= bitsThreshold) {
      webLog("[TRIGGER] BITS (" + String(bitsCount) + ") >= " + String(bitsThreshold) + " — firing output");
      queueFire();
    }
  }

  if (evPointsEnabled && rewardId.length() > 0) {
    if (pointsRewardFilter.length() == 0 || rewardId == pointsRewardFilter) {
      pointsRedemptionCount++;
      webLog("[IRC] Points redemption " + String(pointsRedemptionCount) + "/" + String(pointsThreshold) + " by " + user);
      if (pointsRedemptionCount >= pointsThreshold) {
        pointsRedemptionCount = 0;
        webLog("[TRIGGER] CHANNEL POINTS threshold met — firing output");
        queueFire();
      }
    } else {
      webLog("[IRC] Channel points reward " + rewardId + " does not match filter — ignored");
    }
  }

  if (evSubsEnabled && (msgId == "sub" || msgId == "resub" || msgId == "subgift")) {
    subCount++;
    webLog("[IRC] Sub event " + String(subCount) + "/" + String(subsThreshold) + " from " + user);
    if (subCount >= subsThreshold) {
      subCount = 0;
      webLog("[TRIGGER] SUB threshold met — firing output");
      queueFire();
    }
  }

  if (evRaidsEnabled && msgId == "raid") {
    int viewers = extractTag(msg, "msg-param-viewerCount").toInt();
    webLog("[IRC] RAID from " + user + " viewers=" + String(viewers) + " thresh=" + String(raidThreshold));
    if (viewers >= raidThreshold) {
      webLog("[TRIGGER] RAID threshold met — firing output");
      queueFire();
    }
  }
}

// ── Twitch IRC connection ──────────────────────
void connectTwitch() {
  DPRINTLN("Connecting to Twitch IRC...");
  twitchClient.setInsecure();
  prefs.begin("twitch", true);
  String oauth = prefs.getString("twitch_oauth", TWITCH_OAUTH_SECRET);
  String nick  = prefs.getString("twitch_nick",  TWITCH_OAUTH_NICK);
  prefs.end();
  if (twitchClient.connect("irc.chat.twitch.tv", 6697)) {
    twitchClient.println("PASS " + oauth);
    twitchClient.println("NICK " + nick);
    twitchClient.println("CAP REQ :twitch.tv/tags twitch.tv/commands");
    twitchClient.println("JOIN #" + twitchChannel);
    twitchConnected = true;
    lastTwitchPing = millis();
    DPRINT("Joined #"); DPRINTLN(twitchChannel);
  } else {
    twitchConnected = false;
    DPRINTLN("Twitch connection failed");
  }
}

void handleTwitchIRC() {
  if (!twitchConnected) return;
  while (twitchClient.available()) {
    String line = twitchClient.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    if (!line.startsWith("PING") && !line.startsWith(":tmi.twitch.tv 00")) {
      // Drop raw tag-dump lines (start with @badge)
      if (!line.startsWith("@badge")) {
        // For PRIVMSG/USERNOTICE, only log the parsed summary (done in parseTwitchMessage)
        // For other server messages, log a truncated version
        if (line.indexOf("PRIVMSG") < 0 && line.indexOf("USERNOTICE") < 0) {
          String shortened = line.substring(0, min((int)line.length(), 80));
          webLog("[IRC] " + shortened);
        }
      }
    }

    if (line.startsWith("PING")) {
      twitchClient.println("PONG :tmi.twitch.tv");
      lastTwitchPing = millis();
    } else if (line.indexOf("PRIVMSG") >= 0 || line.indexOf("USERNOTICE") >= 0) {
      parseTwitchMessage(line);
    }
  }
  if (millis() - lastTwitchPing > 240000) {
    twitchClient.println("PING :tmi.twitch.tv");
    lastTwitchPing = millis();
  }
  if (!twitchClient.connected()) {
    twitchConnected = false;
    DPRINTLN("Twitch connection lost");
  }
}

// ── New web routes ─────────────────────────────
void handleResetUsed() {
  usedOutputs = 0;
  nextOutput  = 0;
  pendingUsedQ = -1;
  sendJSON(200, "{\"ok\":true}");
}

void handleUsed() {
  char buf[16];
  snprintf(buf, sizeof(buf), "0x%04X", usedOutputs);
  sendJSON(200, "{\"used\":\"" + String(buf) + "\"}");
}

void handleSaveCfg() {
  if (server.hasArg("bits_threshold"))   bitsThreshold   = max((int)1, (int)server.arg("bits_threshold").toInt());
  if (server.hasArg("points_threshold")) pointsThreshold = max((int)1, (int)server.arg("points_threshold").toInt());
  if (server.hasArg("subs_threshold"))   subsThreshold   = max((int)1, (int)server.arg("subs_threshold").toInt());
  if (server.hasArg("raid_threshold"))   raidThreshold   = max((int)1, (int)server.arg("raid_threshold").toInt());
  if (server.hasArg("pulse_ms")) {
    int ms = server.arg("pulse_ms").toInt();
    if (ms >= 10 && ms <= 30000) pulseDurMs = ms;
  }
  if (server.hasArg("cs_delay_ms")) {
    int d = server.arg("cs_delay_ms").toInt();
    if (d >= 1 && d <= 500) currentSenseDelayMs = d;
  }
  if (server.hasArg("min_gap_ms")) {
    int g = server.arg("min_gap_ms").toInt();
    if (g >= 0 && g <= 60000) minGapMs = g;
  }
  if (server.hasArg("channel")) {
    String ch = server.arg("channel");
    ch.trim();
    if (ch.length() > 0) twitchChannel = ch;
  }
  if (server.hasArg("ev_bits"))   evBitsEnabled   = server.arg("ev_bits") == "1";
  if (server.hasArg("ev_points")) evPointsEnabled = server.arg("ev_points") == "1";
  if (server.hasArg("ev_subs"))   evSubsEnabled   = server.arg("ev_subs") == "1";
  if (server.hasArg("ev_raids"))  evRaidsEnabled  = server.arg("ev_raids") == "1";
  if (server.hasArg("pts_filter")) {
    String pf = server.arg("pts_filter");
    pf.trim();
    pointsRewardFilter = pf;
  }

  prefs.begin("twitch", false);
  prefs.putInt("bitsThreshold", bitsThreshold);
  prefs.putInt("pointsThresh", pointsThreshold);
  prefs.putInt("subsThresh", subsThreshold);
  prefs.putInt("raidThresh", raidThreshold);
  prefs.putUInt("pulseDurMs",  pulseDurMs);
  prefs.putUInt("csDelayMs",  currentSenseDelayMs);
  prefs.putUInt("minGapMs",   minGapMs);
  prefs.putString("channel",   twitchChannel);
  prefs.putBool("evBits",    evBitsEnabled);
  prefs.putBool("evPoints",  evPointsEnabled);
  prefs.putBool("evSubs",    evSubsEnabled);
  prefs.putBool("evRaids",   evRaidsEnabled);
  prefs.putString("ptsFilter", pointsRewardFilter);
  if (server.hasArg("oauth")) {
    String oa = server.arg("oauth");
    if (oa.length() > 0) prefs.putString("twitch_oauth", oa);
  }
  if (server.hasArg("nick")) {
    String nk = server.arg("nick");
    if (nk.length() > 0) prefs.putString("twitch_nick", nk);
  }
  prefs.end();
  sendJSON(200, "{\"ok\":true}");
}

void handleGetCfg() {
  String json = "{\"ok\":true,\"bitsThreshold\":" + String(bitsThreshold) + ",\"pointsThreshold\":" + String(pointsThreshold) + ",\"subsThreshold\":" + String(subsThreshold) + ",\"raidThreshold\":" + String(raidThreshold) + ",\"pulseDurMs\":" + String(pulseDurMs) + ",\"csDelayMs\":" + String(currentSenseDelayMs) + ",\"minGapMs\":" + String(minGapMs) + ",\"twitchConnected\":" + String(twitchConnected ? "true" : "false") + ",\"channel\":\"" + twitchChannel + "\",\"evBits\":" + String(evBitsEnabled ? "true" : "false") + ",\"evPoints\":" + String(evPointsEnabled ? "true" : "false") + ",\"evSubs\":" + String(evSubsEnabled ? "true" : "false") + ",\"evRaids\":" + String(evRaidsEnabled ? "true" : "false") + ",\"ptsFilter\":\"" + pointsRewardFilter + "\",\"nextQ\":" + String(nextOutput) + "}";
  sendJSON(200, json);
}

// ── OTA ───────────────────────────────────────
void setupOTA() {
  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.onStart([]()   { DPRINTLN("OTA start"); });
  ArduinoOTA.onEnd([]()     { DPRINTLN("OTA done");  });
  ArduinoOTA.onError([](ota_error_t e) {
    DPRINT("OTA error: "); DPRINTLN(e);
  });
  ArduinoOTA.begin();
}

// ── Setup ─────────────────────────────────────
void setup() {
#if DEBUG
  Serial.begin(115200);
  delay(200);
#endif
  DPRINTLN("\n\n=== TwitchBlows ===");
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  // 595 pins — initialise latch HIGH before first shiftWrite
  // OE pulled HIGH at boot to keep outputs disabled until we're ready
  pinMode(PIN_DATA,  OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_OE,    OUTPUT);
  digitalWrite(PIN_OE,    HIGH);   // outputs disabled by default
  digitalWrite(PIN_LATCH, HIGH);  // idle state
  disableOutputs();                // all off at boot

  analogSetAttenuation(ADC_11db);
  pinMode(PIN_CURRENT, INPUT);

  // Confirm current sensor is present and idle (expect ~CS_MIDPOINT_V ±0.5V)
  delay(50);
  uint32_t csIdleMv = analogReadMilliVolts(PIN_CURRENT);
  float csIdleV = csIdleMv / 1000.0f;
  bool sensorOK = (csIdleV >= CS_MIDPOINT_V - 0.5f && csIdleV <= CS_MIDPOINT_V + 0.5f);
  webLog("[BOOT] CS idle=" + String(csIdleV, 3) + "V (expect " + String(CS_MIDPOINT_V, 2) + "V) " + (sensorOK ? "OK" : "SENSOR FAULT — outputs DISABLED"));
  if (!sensorOK) {
    digitalWrite(PIN_OE, HIGH);   // disable 595 outputs
  }
  sensorReady = sensorOK;

  prefs.begin("wifi", true);
  String savedSSID = prefs.getString("ssid", MYSSIDIOT);
  String savedPSK  = prefs.getString("psk",  MYPSKIOT);
  prefs.end();

  if (!connectWifi(savedSSID, savedPSK)) {
    DPRINTLN("WiFi failed — starting captive portal AP");
    startAP();
  } else {
    DPRINT("Connected! IP: "); DPRINTLN(WiFi.localIP());
    apMode = false;
    if (MDNS.begin(HOSTNAME)) {
      MDNS.addService("http", "tcp", 80);
      DPRINTLN("mDNS: http://" HOSTNAME ".local");
    }
    setupOTA();

    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    prefs.begin("twitch", true);
    bitsThreshold = prefs.getInt("bitsThreshold", 100);
    pointsThreshold = prefs.getInt("pointsThresh", 1);
    subsThreshold = prefs.getInt("subsThresh", 1);
    raidThreshold = prefs.getInt("raidThresh", 10);
    pulseDurMs   = prefs.getUInt("pulseDurMs", 500);
    currentSenseDelayMs = prefs.getUInt("csDelayMs", 10);
    minGapMs     = prefs.getUInt("minGapMs", 2000);
    twitchChannel = prefs.getString("channel", "daverdavid");
    evBitsEnabled   = prefs.getBool("evBits",   true);
    evPointsEnabled = prefs.getBool("evPoints", true);
    evSubsEnabled   = prefs.getBool("evSubs",   false);
    evRaidsEnabled  = prefs.getBool("evRaids",  false);
    pointsRewardFilter = prefs.getString("ptsFilter", "");
    prefs.end();
  }

  server.on("/",         handleRoot);
  server.on("/set",      handleSet);
  server.on("/pulse",    handlePulse);
  server.on("/state",    handleState);
  server.on("/savewifi", handleSaveWifi);
  server.on("/resetused",  handleResetUsed);
  server.on("/used",       handleUsed);
  server.on("/savecfg",    handleSaveCfg);
  server.on("/getcfg",     handleGetCfg);
  server.on("/log",       handleLog);
  server.onNotFound(     handleNotFound);
  server.begin();
  DPRINTLN("HTTP server started");
}

// ── Loop ──────────────────────────────────────
void loop() {
  if (!apMode) {
    ArduinoOTA.handle();

    // Continuous ADC monitoring — track max over 5 seconds
    if (sensorReady) {
      uint32_t mv = analogReadMilliVolts(PIN_CURRENT);
      uint32_t now = millis();
      if (now - adcMaxTime > 5000) {
          adcMax = 0;          // reset window
          adcMin = 0xFFFFFFFF;  // reset window
          adcMaxTime = now;
          adcMinTime = now;
      }
      if (mv > adcMax) {
          adcMax = mv;        // track peak within window (in mV)
          adcMaxTime = now;
      }
      if (mv < adcMin || adcMin == 0xFFFFFFFF) {
          adcMin = mv;        // track min within window (in mV)
          adcMinTime = now;
      }
      float amps = fabsf(adcToAmps(mv));
      if (now - ampMaxTime > 5000) {
          ampMax = 0.0f;
          ampMin = 1e9f;    // use a large sentinel, not 0.0f
          ampMaxTime = now;
          ampMinTime = now;
      }
      if (amps > ampMax) {
          ampMax = amps;
          ampMaxTime = now;
      }
      if (amps < ampMin) {  // remove the || ampMin == 0.0f guard entirely
          ampMin = amps;
          ampMinTime = now;
      }
    }

    // Pulse auto-off — checked every loop iteration, no blocking delay
    if (pulseActive && (millis() >= pulseEnd)) {
      disableOutputs();
      pulseActive = false;
      pulseQ      = -1;
      activeQ     = -1;
      if (pendingUsedQ >= 0) {
        usedOutputs |= (uint16_t)(1u << pendingUsedQ);
        webLog("[USED] Ch" + String(pendingUsedQ+1) + " marked dead after fire");
        pendingUsedQ = -1;
      }
      webLog("[FIRE] Ch pulse ended — output OFF");
    }

    // Fire queue processor — respects rate limit and waits for pulse to finish
    if (!pulseActive && fireQueueHead != fireQueueTail) {
      uint32_t now = millis();
      if (now - lastFireTime >= minGapMs) {
        webLog("[QUEUE] Dequeuing trigger — firing");
        int result = fireNextOutput(pulseDurMs);
        if (result >= 0) {
          fireQueueHead = (fireQueueHead + 1) % FIRE_QUEUE_SIZE;
          lastFireTime = now;
        } else {
          webLog("[QUEUE] Fire failed — clearing queue");
          fireQueueHead = fireQueueTail;
        }
      }
    }

    // Twitch IRC — connect in background, then poll with auto-reconnect
    static bool twitchInitDone = false;
    if (!twitchInitDone || !twitchConnected) {
      static uint32_t lastTwitchRetry = 0;
      if (millis() - lastTwitchRetry > 10000) {
        lastTwitchRetry = millis();
        twitchInitDone = true;
        connectTwitch();
      }
    } else if (twitchConnected) {
      handleTwitchIRC();
    }

    // WiFi reconnect — debounced to every 5 s so a drop never stalls the loop
    static uint32_t lastWifiCheck = 0;
    if (millis() - lastWifiCheck > 5000) {
      lastWifiCheck = millis();
      if (WiFi.status() != WL_CONNECTED) {
        DPRINTLN("WiFi lost, reconnecting...");
        prefs.begin("wifi", true);
        String ssid = prefs.getString("ssid", MYSSIDIOT);
        String psk  = prefs.getString("psk",  MYPSKIOT);
        prefs.end();
        if (!connectWifi(ssid, psk)) {
          DPRINTLN("Reconnect failed — fallback to AP");
          startAP();
          server.begin();
        }
      }
    }
  } else {
    dns.processNextRequest();
  }

  server.handleClient();
}
