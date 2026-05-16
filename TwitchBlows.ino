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
#define PIN_DATA      1    // DS   (SER)   → 595 pin 14
#define PIN_CLOCK     4    // SRCLK        → 595 pin 11
#define PIN_LATCH     2    // RCLK (ST_CP) → 595 pin 12
#define PIN_OE        5    // OE  (active-low) → 595 pin 13
//           595 pin 10 (SRCLR)→ VCC  (active-low clear, keep high)
//           595 VCC            → 3.3V (match ESP32-C3 logic levels)
//
// Daisy-chain: ESP32 SER → IC1 SER (pin 14)
//              IC1 QH' (pin 9) → IC2 SER (pin 14)
// Bit mapping: val bit 0 = IC1 QA (Ch1) ... bit 7 = IC1 QH (Ch8)
//              val bit 8 = IC2 QA (Ch9) ... bit 15 = IC2 QH (Ch16)
// Low byte sent LAST so it ends up in IC1 (nearest to latch output).

#define PIN_CURRENT   0

// Current sensor calibration — runtime globals (loaded from prefs or these defaults)
float csMidpointV   = 2.5f;    // V at zero current (tune to sensor's actual idle reading)
float csMvPerAmp    = -100.0f; // mV/A sensitivity (ACS712-5A=185, 20A=100, 30A=66; neg if inverted)
float csDetectAmps  = 3.0f;   // minimum |A| to count as live output

// ── ADC Moving Average ────────────────────────
#define ADC_MA_SAMPLES 10   // number of samples for moving average

static uint32_t _adcMaBuf[ADC_MA_SAMPLES] = {0};
static uint8_t  _adcMaIdx = 0;
static bool     _adcMaFilled = false;

uint32_t adcReadMvAvg() {
  _adcMaBuf[_adcMaIdx] = analogReadMilliVolts(PIN_CURRENT);
  _adcMaIdx = (_adcMaIdx + 1) % ADC_MA_SAMPLES;
  if (_adcMaIdx == 0) _adcMaFilled = true;
  uint8_t count = _adcMaFilled ? ADC_MA_SAMPLES : _adcMaIdx;
  if (count == 0) return _adcMaBuf[0];
  uint32_t sum = 0;
  for (uint8_t i = 0; i < count; i++) sum += _adcMaBuf[i];
  return sum / count;
}

// ── Globals ───────────────────────────────────
WebServer   server(80);
DNSServer   dns;
Preferences prefs;

bool    apMode  = false;
int8_t   activeQ = -1;

bool     pulseActive = false;
int8_t   pulseQ      = -1;
uint32_t pulseEnd    = 0;

uint16_t usedOutputs = 0;
uint16_t manualDisableMask = 0;
int      nextOutput  = 0;
int      channelPeak[16] = {0};
float    channelAmpMax[16] = {0.0f};
int8_t  pendingUsedQ = -1;

WiFiClientSecure twitchClient;
bool              twitchConnected = false;
unsigned long     lastTwitchPing = 0;

int     bitsThreshold = 100;
int     pointsThreshold = 1;
int     subsThreshold = 1;
int     raidThreshold = 5;
uint32_t pulseDurMs   = 500;
String  twitchChannel = "daverdavid";

int pointsRedemptionCount = 0;
int subCount = 0;

bool     sensorReady = false;
uint32_t currentSenseDelayMs = 10;
int     adcMax = 0;
uint32_t adcMin = 0xFFFFFFFF;
uint32_t adcMaxTime = 0;
uint32_t adcMinTime = 0;
float   ampMax = 0.0f;
float   ampMin = 1e9f;
uint32_t ampMaxTime = 0;
uint32_t ampMinTime = 0;

bool evBitsEnabled   = true;
bool evPointsEnabled = true;
bool evSubsEnabled   = false;
bool evRaidsEnabled  = false;

#define FIRE_QUEUE_SIZE 16
int8_t  fireQueue[FIRE_QUEUE_SIZE];
int     fireQueueHead = 0;
int     fireQueueTail = 0;

uint32_t minGapMs     = 2000;
uint32_t lastFireTime = 0;

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

// ── Shift register serial debug helper ────────
#if DEBUG
static void debugShiftBytes(const char* label, uint8_t hi, uint8_t lo) {
  char hiBin[9], loBin[9];
  for (int i = 7; i >= 0; i--) {
    hiBin[7 - i] = (hi & (1 << i)) ? '1' : '0';
    loBin[7 - i] = (lo & (1 << i)) ? '1' : '0';
  }
  hiBin[8] = '\0'; loBin[8] = '\0';
  Serial.print("[595] "); Serial.print(label);
  Serial.print("  HI=0b"); Serial.print(hiBin); Serial.print(" (0x"); if (hi < 0x10) Serial.print("0"); Serial.print(hi, HEX);
  Serial.print(") [IC2 Ch9-16]  LO=0b"); Serial.print(loBin); Serial.print(" (0x"); if (lo < 0x10) Serial.print("0"); Serial.print(lo, HEX); Serial.println(") [IC1 Ch1-8]");
}
#else
  #define debugShiftBytes(label, hi, lo)
#endif

// ── 595 helpers ───────────────────────────────
void shiftWrite(uint16_t val) {
  uint8_t hi = (val >> 8) & 0xFF;
  uint8_t lo = val & 0xFF;
  debugShiftBytes("shiftWrite      ", hi, lo);
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, hi);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, lo);
  digitalWrite(PIN_LATCH, HIGH);
}

void shiftWriteEnabled(uint16_t val) {
  uint8_t hi = (val >> 8) & 0xFF;
  uint8_t lo = val & 0xFF;
  debugShiftBytes("shiftWriteEnabled", hi, lo);
  digitalWrite(PIN_OE, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  delayMicroseconds(5);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, hi);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, lo);
  digitalWrite(PIN_LATCH, HIGH);
  if (val != 0) digitalWrite(PIN_OE, LOW);
}

void disableOutputs() {
  debugShiftBytes("disableOutputs  ", 0x00, 0x00);
  digitalWrite(PIN_OE, HIGH);
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0x00);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, 0x00);
  digitalWrite(PIN_LATCH, HIGH);
}

float adcToAmps(int millivolts) {
  float v = millivolts / 1000.0f;
  return (v - csMidpointV) / (csMvPerAmp / 1000.0f);
}

void safePulse(int8_t q, uint32_t ms) {
  disableOutputs();
  pulseActive = false; pulseQ = -1; activeQ = -1;
  if (q < 0 || q > 15) return;
  shiftWriteEnabled((uint16_t)(1u << q));
  float peakAmps = 0.0f;
  if (sensorReady) {
    uint32_t senseStart = millis();
    while (millis() - senseStart < currentSenseDelayMs) {
      int mv = adcReadMvAvg();
      float a = fabsf(adcToAmps(mv));
      if (a > peakAmps) peakAmps = a;
    }
  }
  channelPeak[q] = (int)(peakAmps * 1000.0f);
  channelAmpMax[q] = max(channelAmpMax[q], peakAmps);
  pulseActive = true; pulseQ = q; activeQ = q; pulseEnd = millis() + ms;
  webLog("[FIRE] Ch" + String(q+1) + " ON for " + String(ms) + "ms");
}

int fireNextOutput(uint32_t pulseDurationMs) {
  if (!sensorReady) { webLog("[FIRE] Blocked — sensor not ready"); return -1; }
  disableOutputs();
  for (int tries = 0; tries < 16; tries++) {
    int q = nextOutput;
    nextOutput = (nextOutput + 1) % 16;
    if (usedOutputs & (uint16_t)(1u << q)) { webLog("[FIRE] Ch" + String(q+1) + " already used — skip"); continue; }
    if (manualDisableMask & (uint16_t)(1u << q)) { webLog("[FIRE] Ch" + String(q+1) + " manually disabled — skip"); continue; }
    shiftWriteEnabled((uint16_t)(1u << q));
    float peakAmps = 0.0f;
    uint32_t senseStart = millis();
    while (millis() - senseStart < currentSenseDelayMs) {
      int mv = adcReadMvAvg();
      float a = fabsf(adcToAmps(mv));
      if (a > peakAmps) peakAmps = a;
    }
    channelPeak[q] = (int)(peakAmps * 1000.0f);
    channelAmpMax[q] = max(channelAmpMax[q], peakAmps);
    if (peakAmps < csDetectAmps) {
      disableOutputs();
      usedOutputs |= (uint16_t)(1u << q);
      webLog("[Ch" + String(q+1) + "] peak=" + String(peakAmps,3) + "A — DEAD");
      continue;
    }
    pulseActive = true; pulseQ = q; activeQ = q; pulseEnd = millis() + pulseDurationMs; pendingUsedQ = q;
    webLog("[Ch" + String(q+1) + "] peak=" + String(peakAmps,3) + "A — FIRING " + String(pulseDurationMs) + "ms");
    return q;
  }
  webLog("[FIRE] All 16 outputs exhausted");
  disableOutputs();
  return -1;
}

void queueFire() {
  int next = (fireQueueTail + 1) % FIRE_QUEUE_SIZE;
  if (next == fireQueueHead) { webLog("[QUEUE] Full — trigger dropped"); return; }
  fireQueue[fireQueueTail] = 1;
  fireQueueTail = next;
  webLog("[QUEUE] Trigger enqueued (" + String((fireQueueTail - fireQueueHead + FIRE_QUEUE_SIZE) % FIRE_QUEUE_SIZE) + " pending)");
}

void sendJSON(int code, const String &json) {
  server.sendHeader("Connection", "keep-alive");
  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(code, "application/json", json);
}

// ── WiFi ──────────────────────────────────────
bool connectWifi(const String &ssid, const String &psk) {
  WiFi.persistent(false); WiFi.setAutoReconnect(true); WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_8_5dBm); WiFi.setHostname(HOSTNAME);
  WiFi.begin(ssid.c_str(), psk.c_str()); delay(500); WiFi.disconnect(true); delay(200);
  for (int attempt = 0; attempt < 3; attempt++) {
    DPRINT("Connecting to "); DPRINT(ssid); DPRINT(" attempt "); DPRINTLN(attempt + 1);
    WiFi.begin(ssid.c_str(), psk.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) { delay(100); DPRINT("."); }
    if (WiFi.status() == WL_CONNECTED) { DPRINTLN(""); return true; }
    WiFi.disconnect(true); delay(500);
  }
  DPRINTLN("failed"); return false;
}

void startAP() {
  apMode = true; WiFi.mode(WIFI_AP); WiFi.setTxPower(WIFI_POWER_11dBm);
  WiFi.softAP(AP_SSID); dns.start(53, "*", WiFi.softAPIP());
  DPRINT("AP started: "); DPRINTLN(AP_SSID);
  DPRINT("AP IP: ");      DPRINTLN(WiFi.softAPIP());
}

// ── Web routes ────────────────────────────────
void handleRoot() {
  if (apMode) server.send_P(200, "text/html", PORTAL_HTML);
  else        server.send_P(200, "text/html", INDEX_HTML);
}

void handleConfig() {
  server.send_P(200, "text/html", CONFIG_HTML);
}

void handleSet() {
  if (server.hasArg("q")) {
    int q = server.arg("q").toInt();
    if (q < 0 || q > 15) q = -1;
    if (q < 0) disableOutputs();
    else safePulse((int8_t)q, 500);
  }
  sendJSON(200, "{\"active\":" + String(activeQ) + "}");
}

void handlePulse() {
  if (!server.hasArg("q") || !server.hasArg("ms")) { sendJSON(400, "{\"ok\":false,\"err\":\"missing q or ms\"}"); return; }
  int q  = server.arg("q").toInt();
  int ms = constrain(server.arg("ms").toInt(), 10, 30000);
  if (q < 0 || q > 15) { sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}"); return; }
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
                ",\"adcCurr\":" + String(sensorReady ? adcReadMvAvg() : 0) +
                ",\"adcMax\":" + String(adcMax) +
                ",\"adcMin\":" + String(adcMin) +
                ",\"ampCurr\":" + String(sensorReady ? String(fabsf(adcToAmps(adcReadMvAvg())), 3) : "0") +
                ",\"ampMax\":" + String(ampMax, 3) +
                ",\"ampMin\":" + String(ampMin, 3) +
                ",\"peaks\":[";
  for (int i = 0; i < 16; i++) { json += String(channelPeak[i]); if (i < 15) json += ","; }
  json += "],\"ampPeaks\":[";
  for (int i = 0; i < 16; i++) { json += String(channelAmpMax[i], 3); if (i < 15) json += ","; }
  json += "]}";
  sendJSON(200, json);
}

void handleSetChan() {
  if (!server.hasArg("q")) { sendJSON(400, "{\"ok\":false,\"err\":\"missing q\"}"); return; }
  int q = server.arg("q").toInt();
  if (q < 0 || q > 15) { sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}"); return; }
  bool enabled = !server.hasArg("enabled") || server.arg("enabled") == "1";
  if (enabled) manualDisableMask &= ~(1u << q);
  else         manualDisableMask |= (1u << q);
  sendJSON(200, "{\"ok\":true,\"q\":" + String(q) + ",\"enabled\":" + (enabled ? "true" : "false") + "}");
}

void handleSetDead() {
  if (!server.hasArg("q")) { sendJSON(400, "{\"ok\":false,\"err\":\"missing q\"}"); return; }
  int q = server.arg("q").toInt();
  if (q < 0 || q > 15) { sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}"); return; }
  bool dead = server.hasArg("dead") && server.arg("dead") == "1";
  if (dead) usedOutputs |= (uint16_t)(1u << q);
  else      usedOutputs &= ~(uint16_t)(1u << q);
  sendJSON(200, "{\"ok\":true,\"q\":" + String(q) + ",\"dead\":" + String(dead ? "true" : "false") + "}");
}

void handleGetChan() {
  sendJSON(200, "{\"disableMask\":" + String(manualDisableMask) + "}");
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
    delay(1500); ESP.restart();
  } else {
    server.sendHeader("Location", "/"); server.send(302);
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
  payload.replace("\r", ""); payload.trim(); return payload;
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

  static const char* knownBots[] = {"Nightbot","StreamElements","Moobot","Fossabot","Streamlabs","CommanderRoot",nullptr};
  bool isBot = false;
  for (int i = 0; knownBots[i] != nullptr; i++) { if (user.equalsIgnoreCase(knownBots[i])) { isBot = true; break; } }
  if (isBot) { webLog("[IRC] (bot) " + user + " ignored"); return; }

  if (bitsStr.length() > 0 && bitsStr != "0") webLog("[IRC] BITS event — user=" + user + " bits=" + bitsStr);
  if (rewardId.length() > 0) webLog("[IRC] CHANNEL POINTS — user=" + user + " reward-id=" + rewardId);
  if (msgId == "sub" || msgId == "resub" || msgId == "subgift") webLog("[IRC] SUB event — user=" + user + " msg-id=" + msgId);
  if (msgId == "raid") webLog("[IRC] RAID event — user=" + user);
  if (bitsStr.length() == 0 && rewardId.length() == 0 && msgId.length() == 0) webLog("[IRC] CHAT msg — user=" + user + " msg=" + extractIRCMessage(msg));

  if (evBitsEnabled && bitsStr.length() > 0) {
    int bitsCount = bitsStr.toInt();
    if (bitsCount > 0 && bitsCount >= bitsThreshold) { webLog("[TRIGGER] BITS (" + String(bitsCount) + ") >= " + String(bitsThreshold) + " — firing output"); queueFire(); }
  }
  if (evPointsEnabled && rewardId.length() > 0) {
    if (pointsRewardFilter.length() == 0 || rewardId == pointsRewardFilter) {
      pointsRedemptionCount++;
      webLog("[IRC] Points redemption " + String(pointsRedemptionCount) + "/" + String(pointsThreshold) + " by " + user);
      if (pointsRedemptionCount >= pointsThreshold) { pointsRedemptionCount = 0; webLog("[TRIGGER] CHANNEL POINTS threshold met — firing output"); queueFire(); }
    } else { webLog("[IRC] Channel points reward " + rewardId + " does not match filter — ignored"); }
  }
  if (evSubsEnabled && (msgId == "sub" || msgId == "resub" || msgId == "subgift")) {
    subCount++;
    webLog("[IRC] Sub event " + String(subCount) + "/" + String(subsThreshold) + " from " + user);
    if (subCount >= subsThreshold) { subCount = 0; webLog("[TRIGGER] SUB threshold met — firing output"); queueFire(); }
  }
  if (evRaidsEnabled && msgId == "raid") {
    int viewers = extractTag(msg, "msg-param-viewerCount").toInt();
    webLog("[IRC] RAID from " + user + " viewers=" + String(viewers) + " thresh=" + String(raidThreshold));
    if (viewers >= raidThreshold) { webLog("[TRIGGER] RAID threshold met — firing output"); queueFire(); }
  }
}

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
    twitchConnected = true; lastTwitchPing = millis();
    DPRINT("Joined #"); DPRINTLN(twitchChannel);
  } else {
    twitchConnected = false; DPRINTLN("Twitch connection failed");
  }
}

void handleTwitchIRC() {
  if (!twitchConnected) return;
  while (twitchClient.available()) {
    String line = twitchClient.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    if (!line.startsWith("PING") && !line.startsWith(":tmi.twitch.tv 00")) {
      if (!line.startsWith("@badge")) {
        if (line.indexOf("PRIVMSG") < 0 && line.indexOf("USERNOTICE") < 0) {
          webLog("[IRC] " + line.substring(0, min((int)line.length(), 80)));
        }
      }
    }
    if (line.startsWith("PING")) { twitchClient.println("PONG :tmi.twitch.tv"); lastTwitchPing = millis(); }
    else if (line.indexOf("PRIVMSG") >= 0 || line.indexOf("USERNOTICE") >= 0) parseTwitchMessage(line);
  }
  if (millis() - lastTwitchPing > 240000) { twitchClient.println("PING :tmi.twitch.tv"); lastTwitchPing = millis(); }
  if (!twitchClient.connected()) { twitchConnected = false; DPRINTLN("Twitch connection lost"); }
}

void handleResetUsed() { usedOutputs = 0; sendJSON(200, "{\"ok\":true}"); }
void handleDisableAll() { usedOutputs = 0xFFFF; sendJSON(200, "{\"ok\":true}"); }
void handleUsed() { char buf[16]; snprintf(buf, sizeof(buf), "0x%04X", usedOutputs); sendJSON(200, "{\"used\":\"" + String(buf) + "\"}"); }

void handleSaveCfg() {
  if (server.hasArg("bits_threshold"))   bitsThreshold   = max((int)1, (int)server.arg("bits_threshold").toInt());
  if (server.hasArg("points_threshold")) pointsThreshold = max((int)1, (int)server.arg("points_threshold").toInt());
  if (server.hasArg("subs_threshold"))   subsThreshold   = max((int)1, (int)server.arg("subs_threshold").toInt());
  if (server.hasArg("raid_threshold"))   raidThreshold   = max((int)1, (int)server.arg("raid_threshold").toInt());
  if (server.hasArg("pulse_ms")) { int ms = server.arg("pulse_ms").toInt(); if (ms >= 10 && ms <= 30000) pulseDurMs = ms; }
  if (server.hasArg("cs_delay_ms")) { int d = server.arg("cs_delay_ms").toInt(); if (d >= 1 && d <= 500) currentSenseDelayMs = d; }
  if (server.hasArg("min_gap_ms")) { int g = server.arg("min_gap_ms").toInt(); if (g >= 0 && g <= 60000) minGapMs = g; }
  if (server.hasArg("cs_midpoint_v")) { float v = server.arg("cs_midpoint_v").toFloat(); if (v >= 0.0f && v <= 3.3f) csMidpointV = v; }
  if (server.hasArg("cs_mv_per_amp")) { float v = server.arg("cs_mv_per_amp").toFloat(); if (v != 0.0f) csMvPerAmp = v; }
  if (server.hasArg("cs_detect_amps")) { float v = server.arg("cs_detect_amps").toFloat(); if (v > 0.0f && v <= 50.0f) csDetectAmps = v; }
  if (server.hasArg("channel")) { String ch = server.arg("channel"); ch.trim(); if (ch.length() > 0) twitchChannel = ch; }
  if (server.hasArg("ev_bits"))   evBitsEnabled   = server.arg("ev_bits") == "1";
  if (server.hasArg("ev_points")) evPointsEnabled = server.arg("ev_points") == "1";
  if (server.hasArg("ev_subs"))   evSubsEnabled   = server.arg("ev_subs") == "1";
  if (server.hasArg("ev_raids"))  evRaidsEnabled  = server.arg("ev_raids") == "1";
  if (server.hasArg("pts_filter")) { String pf = server.arg("pts_filter"); pf.trim(); poi