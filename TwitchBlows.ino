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
#include "html.h"

// ── Configuration ─────────────────────────────
#define HOSTNAME      "twitchblows"
#define WIFI_TIMEOUT  15000      // ms to wait for STA connection
#define AP_SSID       HOSTNAME   // Captive portal AP name

// SN74HC595 pins (adjust to your wiring)
#define PIN_DATA      4    // DS   (SER)   → 595 pin 14
#define PIN_CLOCK     6    // SRCLK        → 595 pin 11
#define PIN_LATCH     5    // RCLK (ST_CP) → 595 pin 12
#define PIN_OE        3    // OE  (active-low) → 595 pin 13
//           595 pin 10 (SRCLR)→ VCC  (active-low clear, keep high)
//           595 VCC            → 3.3V (match ESP32-C3 logic levels)

#define PIN_CURRENT   0   // GPIO0 — change to whichever ADC-capable pin you're using

// Current sensor calibration — ACS-style, midpoint ~1.65V on 3.3V/12-bit ADC
#define CS_MIDPOINT_V     1.6f   // V at zero current (tune to your sensor's actual idle reading)
#define CS_MV_PER_AMP    064.0f   // mV/A sensitivity (e.g. ACS712-5A=185, 20A=100, 30A=66)
#define CS_ADC_REF_V      3.3f    // ADC reference voltage
#define CS_ADC_BITS      4095    // 12-bit ADC max count
#define CS_DETECT_AMPS  1.9f    // minimum current (A) to count as live output

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
uint32_t pulseDurMs   = 500;     // how long each output fires (ms)
String  twitchChannel = "daverdavid";  // loaded from prefs

// Current sense config
bool     sensorReady = false;   // set true after boot check passes
uint32_t currentSenseDelayMs = 10;  // ms to wait after firing before reading ADC

// Event enable flags
bool evBitsEnabled   = true;
bool evPointsEnabled = true;
bool evSubsEnabled   = false;
bool evRaidsEnabled  = false;

// Channel points reward ID filter (empty = trigger on ALL redemptions)
String pointsRewardFilter = "";

// ── Web console log buffer ─────────────────────
#define LOG_LINES 40
#define LOG_WIDTH 120
char logBuf[LOG_LINES][LOG_WIDTH];
int  logHead = 0;
int  logCount = 0;

void webLog(const String& msg) {
  DPRINTLN(msg);
  msg.toCharArray(logBuf[logHead], LOG_WIDTH - 1);
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

float adcToAmps(int raw) {
  float v = (raw / (float)CS_ADC_BITS) * CS_ADC_REF_V;
  return (v - CS_MIDPOINT_V) / (CS_MV_PER_AMP / 1000.0f);
}

void setOutput(int8_t q) {
  activeQ = q;
  shiftWrite((q >= 0) ? (uint16_t)(1 << q) : 0);
  webLog("[TOGGLE] Ch" + String(q+1) + " " + (q >= 0 ? "ON" : "OFF"));
}

// ── Single-output enforcer ────────────────────
void clearAllOutputs() {
  shiftWrite(0);
  pulseActive = false;
  pulseQ      = -1;
  activeQ     = -1;
}

// ── Safe pulse: guaranteed single-output, always-off-after ─
void safePulse(int8_t q, uint32_t ms) {
  clearAllOutputs();
  if (q < 0 || q > 15) return;
  shiftWrite((uint16_t)(1u << q));
  pulseActive = true;
  pulseQ      = q;
  pulseEnd    = millis() + ms;
}

// ── Fire next live output with current sensing ─
int fireNextOutput(uint32_t pulseDurationMs) {
  if (!sensorReady) { webLog("[FIRE] Blocked — sensor not ready"); return -1; }

  clearAllOutputs();

  for (int tries = 0; tries < 16; tries++) {
    int q = nextOutput;
    nextOutput = (nextOutput + 1) % 16;

    if (usedOutputs & (uint16_t)(1u << q)) {
      webLog("[FIRE] Ch" + String(q+1) + " already used — skip");
      continue;
    }

    shiftWrite((uint16_t)(1u << q));
    webLog("[FIRE] Ch" + String(q+1) + " — firing output...");

    // Sample peak deviation from midpoint over sense window
    float peakAmps = 0.0f;
    uint32_t senseStart = millis();
    while (millis() - senseStart < currentSenseDelayMs) {
      int raw = analogRead(PIN_CURRENT);
      float a = fabsf(adcToAmps(raw));
      if (a > peakAmps) peakAmps = a;
    }
    channelPeak[q] = (int)(peakAmps * 1000.0f);  // store as mA

    webLog("[SENSE] Ch" + String(q+1) + " peak=" + String(peakAmps, 3) + "A");

    // Deviation check: voltage must move >0.35V from midpoint
    float csThreshAmps = CS_DETECT_AMPS;
    if (peakAmps < csThreshAmps) {
      shiftWrite(0);
      usedOutputs |= (uint16_t)(1u << q);
      webLog("[FIRE] Ch" + String(q+1) + " DEAD (" + String(peakAmps,3) + "A < " + String(csThreshAmps,3) + "A)");
      continue;
    }

    // Use safePulse for guaranteed single-output, always-off-after
    safePulse(q, pulseDurationMs);
    pendingUsedQ = q;
    webLog("[FIRE] Ch" + String(q+1) + " CONFIRMED LIVE — firing " + String(pulseDurationMs) + "ms");
    return q;
  }

  webLog("[FIRE] All 16 outputs exhausted");
  clearAllOutputs();
  return -1;
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
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_11dBm);
  WiFi.setHostname(HOSTNAME);
  WiFi.begin(ssid.c_str(), psk.c_str());
  DPRINT("Connecting to "); DPRINTLN(ssid);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_TIMEOUT) {
    delay(250); DPRINT(".");
  }
  DPRINTLN("");
  return WiFi.status() == WL_CONNECTED;
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
  clearAllOutputs();

  if (server.hasArg("q")) {
    int q = server.arg("q").toInt();
    if (q < 0 || q > 15) q = -1;
    setOutput((int8_t)q);
  }
  sendJSON(200, "{\"active\":" + String(activeQ) + "}");
}

// /pulse?q=N&ms=M  — raise output N for M ms then auto-drop in loop()
void handlePulse() {
  if (!sensorReady) { sendJSON(503, "{\"ok\":false,\"err\":\"sensor not ready\"}"); return; }
  if (!server.hasArg("q") || !server.hasArg("ms")) {
    sendJSON(400, "{\"ok\":false,\"err\":\"missing q or ms\"}");
    return;
  }

  int q  = server.arg("q").toInt();
  int ms = server.arg("ms").toInt();

  if (q < 0 || q > 15) {
    sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}");
    return;
  }
  if (ms < 10)    ms = 10;
  if (ms > 30000) ms = 30000;

  clearAllOutputs();
  shiftWrite((uint16_t)(1u << q));
  pulseActive = true;
  pulseQ      = (int8_t)q;
  pulseEnd    = millis() + (uint32_t)ms;

  webLog("[PULSE] Ch" + String(q+1) + " for " + String(ms) + "ms");

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
      fireNextOutput(pulseDurMs);
    }
  }

  if (evPointsEnabled && rewardId.length() > 0) {
    if (pointsRewardFilter.length() == 0 || rewardId == pointsRewardFilter) {
      webLog("[TRIGGER] CHANNEL POINTS by " + user + " — firing output");
      fireNextOutput(pulseDurMs);
    } else {
      webLog("[IRC] Channel points reward " + rewardId + " does not match filter — ignored");
    }
  }

  if (evSubsEnabled && (msgId == "sub" || msgId == "resub" || msgId == "subgift")) {
    webLog("[TRIGGER] SUB by " + user + " — firing output");
    fireNextOutput(pulseDurMs);
  }

  if (evRaidsEnabled && msgId == "raid") {
    webLog("[TRIGGER] RAID from " + user + " — firing output");
    fireNextOutput(pulseDurMs);
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
      webLog("[IRC] " + line);
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
  sendJSON(200, "{\"ok\":true}");
}

void handleUsed() {
  char buf[16];
  snprintf(buf, sizeof(buf), "0x%04X", usedOutputs);
  sendJSON(200, "{\"used\":\"" + String(buf) + "\"}");
}

void handleSaveCfg() {
  if (server.hasArg("bits_threshold")) {
    bitsThreshold = server.arg("bits_threshold").toInt();
    if (bitsThreshold < 1) bitsThreshold = 1;
  }
  if (server.hasArg("pulse_ms")) {
    int ms = server.arg("pulse_ms").toInt();
    if (ms >= 10 && ms <= 30000) pulseDurMs = ms;
  }
  if (server.hasArg("cs_delay_ms")) {
    int d = server.arg("cs_delay_ms").toInt();
    if (d >= 1 && d <= 500) currentSenseDelayMs = d;
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
  prefs.putUInt("pulseDurMs",  pulseDurMs);
  prefs.putUInt("csDelayMs",  currentSenseDelayMs);
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
  String json = "{\"ok\":true,\"bitsThreshold\":" + String(bitsThreshold) + ",\"pulseDurMs\":" + String(pulseDurMs) + ",\"csDelayMs\":" + String(currentSenseDelayMs) + ",\"twitchConnected\":" + String(twitchConnected ? "true" : "false") + ",\"channel\":\"" + twitchChannel + "\",\"evBits\":" + String(evBitsEnabled ? "true" : "false") + ",\"evPoints\":" + String(evPointsEnabled ? "true" : "false") + ",\"evSubs\":" + String(evSubsEnabled ? "true" : "false") + ",\"evRaids\":" + String(evRaidsEnabled ? "true" : "false") + ",\"ptsFilter\":\"" + pointsRewardFilter + "\",\"nextQ\":" + String(nextOutput) + "}";
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

  // 595 pins — initialise latch HIGH before first shiftWrite
  // OE pulled HIGH at boot to keep outputs disabled until we're ready
  pinMode(PIN_DATA,  OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  digitalWrite(PIN_OE,    HIGH);   // disable outputs during init
  pinMode(PIN_OE,    OUTPUT);
  digitalWrite(PIN_LATCH, HIGH);  // idle state
  setOutput(-1);                  // all off at boot

  // Enable 595 outputs after init is complete
  digitalWrite(PIN_OE, LOW);

  pinMode(PIN_CURRENT, INPUT);

  // Confirm current sensor is present and idle (expect ~1.65V ±0.3V)
  delay(50);
  int csIdle = analogRead(PIN_CURRENT);
  float csIdleV = (csIdle / (float)CS_ADC_BITS) * CS_ADC_REF_V;
  bool sensorOK = (csIdleV >= 1.2f && csIdleV <= 2.1f);
  webLog("[BOOT] CS idle=" + String(csIdleV, 3) + "V " + (sensorOK ? "OK" : "SENSOR FAULT — outputs DISABLED"));
  if (!sensorOK) {
    digitalWrite(PIN_OE, HIGH);   // disable 595 outputs
  }
  sensorReady = sensorOK;

  prefs.begin("wifi", true);
  String savedSSID = prefs.getString("ssid", MYSSID);
  String savedPSK  = prefs.getString("psk",  MYPSK);
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

    prefs.begin("twitch", true);
    bitsThreshold = prefs.getInt("bitsThreshold", 100);
    pulseDurMs   = prefs.getUInt("pulseDurMs",   500);
    currentSenseDelayMs = prefs.getUInt("csDelayMs", 10);
    twitchChannel = prefs.getString("channel", "daverdavid");
    evBitsEnabled   = prefs.getBool("evBits",   true);
    evPointsEnabled = prefs.getBool("evPoints", true);
    evSubsEnabled   = prefs.getBool("evSubs",   false);
    evRaidsEnabled  = prefs.getBool("evRaids",  false);
    pointsRewardFilter = prefs.getString("ptsFilter", "");
    prefs.end();

    connectTwitch();
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

    // Pulse auto-off — checked every loop iteration, no blocking delay
    if (pulseActive && (millis() >= pulseEnd)) {
      shiftWrite(0);
      pulseActive = false;
      pulseQ      = -1;
      if (pendingUsedQ >= 0) {
        usedOutputs |= (uint16_t)(1u << pendingUsedQ);
        webLog("[USED] Ch" + String(pendingUsedQ+1) + " marked dead after fire");
        pendingUsedQ = -1;
      }
      webLog("[PULSE] ended — output OFF");
    }

    // Twitch IRC polling with auto-reconnect
    if (twitchConnected) {
      handleTwitchIRC();
    } else {
      static uint32_t lastTwitchRetry = 0;
      if (millis() - lastTwitchRetry > 10000) {
        lastTwitchRetry = millis();
        connectTwitch();
      }
    }

    // WiFi reconnect — debounced to every 5 s so a drop never stalls the loop
    static uint32_t lastWifiCheck = 0;
    if (millis() - lastWifiCheck > 5000) {
      lastWifiCheck = millis();
      if (WiFi.status() != WL_CONNECTED) {
        DPRINTLN("WiFi lost, reconnecting...");
        prefs.begin("wifi", true);
        String ssid = prefs.getString("ssid", MYSSID);
        String psk  = prefs.getString("psk",  MYPSK);
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
