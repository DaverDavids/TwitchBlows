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

#define PIN_CURRENT   A0   // Analog pin for current sense

#define TWITCH_CHANNEL "daverdavid"   // set your Twitch channel here

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

// Twitch IRC
WiFiClientSecure twitchClient;
bool              twitchConnected = false;
unsigned long     lastTwitchPing = 0;

// Twitch trigger config
int     bitsThreshold = 100;    // bits needed to trigger one output
uint32_t pulseDurMs   = 500;     // how long each output fires (ms)

// ── 595 helper ────────────────────────────────
void shiftWrite(uint16_t val) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, (val >> 8) & 0xFF);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, val & 0xFF);
  digitalWrite(PIN_LATCH, HIGH);
}

void setOutput(int8_t q) {
  activeQ = q;
  shiftWrite((q >= 0) ? (uint16_t)(1 << q) : 0);
  DPRINT("Output set to Q"); DPRINTLN(q);
}

// Fire next unused output, sense current, mark dead if no response
int fireNextOutput(uint32_t pulseDurationMs) {
  for (int tries = 0; tries < 16; tries++) {
    int q = nextOutput;
    nextOutput = (nextOutput + 1) % 16;
    if (usedOutputs & (1 << q)) continue;

    shiftWrite((uint16_t)(1 << q));
    delayMicroseconds(500);

    int raw = analogRead(PIN_CURRENT);
    if (raw < 10) {
      shiftWrite(0);
      usedOutputs |= (1 << q);
      DPRINT("Output Q"); DPRINT(q); DPRINTLN(" dead (no current) — skipped");
      continue;
    }

    pulseActive = true;
    pulseQ       = (int8_t)q;
    pulseEnd     = millis() + pulseDurationMs;
    DPRINT("Fired Q"); DPRINTLN(q);
    return q;
  }
  DPRINTLN("All 16 outputs used/dead");
  shiftWrite(0);
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
  pulseActive = false;
  pulseQ      = -1;

  if (server.hasArg("q")) {
    int q = server.arg("q").toInt();
    if (q < 0 || q > 15) q = -1;
    setOutput((int8_t)q);
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
  int ms = server.arg("ms").toInt();

  if (q < 0 || q > 15) {
    sendJSON(400, "{\"ok\":false,\"err\":\"q out of range\"}");
    return;
  }
  if (ms < 10)    ms = 10;
  if (ms > 30000) ms = 30000;

  activeQ = -1;
  shiftWrite((uint16_t)(1 << q));
  pulseActive = true;
  pulseQ      = (int8_t)q;
  pulseEnd    = millis() + (uint32_t)ms;

  DPRINT("Pulse Q"); DPRINT(q); DPRINT(" for "); DPRINT(ms); DPRINTLN("ms");

  sendJSON(200, "{\"ok\":true,\"q\":" + String(q) + ",\"ms\":" + String(ms) + "}");
}

void handleState() {
  String json = "{\"active\":" + String(activeQ) +
                ",\"pulse\":"  + (pulseActive ? "true" : "false") +
                ",\"pulseQ\":" + String(pulseActive ? pulseQ : -1) + "}";
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
  if (msg.indexOf("bits=") > 0) {
    String bitsStr = extractTag(msg, "bits");
    int bitsCount  = bitsStr.toInt();
    if (bitsCount >= bitsThreshold) {
      fireNextOutput(pulseDurMs);
    }
  }
}

// ── Twitch IRC connection ──────────────────────
void connectTwitch() {
  DPRINTLN("Connecting to Twitch IRC...");
  twitchClient.setInsecure();
  if (twitchClient.connect("irc.chat.twitch.tv", 6697)) {
    twitchClient.println("PASS " + String(TWITCH_OAUTH_SECRET));
    twitchClient.println("NICK " + String(TWITCH_OAUTH_NICK));
    twitchClient.println("CAP REQ :twitch.tv/tags twitch.tv/commands");
    twitchClient.println("JOIN #" TWITCH_CHANNEL);
    twitchConnected = true;
    lastTwitchPing = millis();
    DPRINT("Joined #"); DPRINTLN(TWITCH_CHANNEL);
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
  prefs.begin("twitch", false);
  prefs.putInt("bitsThreshold", bitsThreshold);
  prefs.putUInt("pulseDurMs",  pulseDurMs);
  prefs.end();
  sendJSON(200, "{\"ok\":true,\"bitsThreshold\":" + String(bitsThreshold) + ",\"pulseDurMs\":" + String(pulseDurMs) + "}");
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
  pinMode(PIN_OE,    OUTPUT);
  digitalWrite(PIN_OE,    HIGH);   // disable outputs during init
  digitalWrite(PIN_LATCH, HIGH);  // idle state
  setOutput(-1);                  // all off at boot

  // Enable 595 outputs after init is complete
  digitalWrite(PIN_OE, LOW);

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
    connectTwitch();

    prefs.begin("twitch", true);
    bitsThreshold = prefs.getInt("bitsThreshold", 100);
    pulseDurMs   = prefs.getUInt("pulseDurMs",   500);
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
      DPRINTLN("Pulse ended — output OFF");
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
