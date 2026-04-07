// ─────────────────────────────────────────────
//  TwitchBlows — ESP32-C3  +  SN74HC595
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
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "Secrets.h"
#include "html.h"

// ── Configuration ─────────────────────────────
#define HOSTNAME      "twitchblows"
#define WIFI_TIMEOUT  15000      // ms to wait for STA connection

#define AP_SSID       HOSTNAME   // Captive portal AP name

// SN74HC595 pins (adjust to your wiring)
#define PIN_DATA      4    // DS   (SER)
#define PIN_CLOCK     5    // SRCLK
#define PIN_LATCH     6    // RCLK (ST_CP)

// ── Globals ───────────────────────────────────
WebServer  server(80);
DNSServer  dns;
Preferences prefs;

bool       apMode   = false;
int8_t     activeQ  = -1;   // -1 = all off, 0-7 = active output (toggle)

// Pulse state
bool       pulseActive = false;
int8_t     pulseQ      = -1;
uint32_t   pulseEnd    = 0;   // millis() when pulse should end

// ── 595 helper ────────────────────────────────
void shiftWrite(uint8_t val) {
  digitalWrite(PIN_LATCH, LOW);
  shiftOut(PIN_DATA, PIN_CLOCK, MSBFIRST, val);
  digitalWrite(PIN_LATCH, HIGH);
}

void setOutput(int8_t q) {
  activeQ = q;
  shiftWrite((q >= 0) ? (1 << q) : 0);
  DPRINT("Output set to Q"); DPRINTLN(q);
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
  DPRINT("AP IP: "); DPRINTLN(WiFi.softAPIP());
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
  // Cancel any active pulse when explicitly setting an output
  pulseActive = false;
  pulseQ      = -1;

  if (server.hasArg("q")) {
    int q = server.arg("q").toInt();
    if (q < 0 || q > 7) q = -1;
    setOutput((int8_t)q);
  }
  String json = "{\"active\":" + String(activeQ) + "}";
  server.send(200, "application/json", json);
}

// /pulse?q=N&ms=M  — raise output N for M milliseconds then drop it
void handlePulse() {
  if (!server.hasArg("q") || !server.hasArg("ms")) {
    server.send(400, "application/json", "{\"ok\":false,\"err\":\"missing q or ms\"}");
    return;
  }

  int q  = server.arg("q").toInt();
  int ms = server.arg("ms").toInt();

  if (q < 0 || q > 7) {
    server.send(400, "application/json", "{\"ok\":false,\"err\":\"q out of range\"}");
    return;
  }
  if (ms < 10)    ms = 10;
  if (ms > 30000) ms = 30000;

  // Cancel any current toggle or previous pulse first
  activeQ = -1;

  // Raise the pin
  shiftWrite(1 << q);
  pulseActive = true;
  pulseQ      = (int8_t)q;
  pulseEnd    = millis() + (uint32_t)ms;

  DPRINT("Pulse Q"); DPRINT(q); DPRINT(" for "); DPRINT(ms); DPRINTLN("ms");

  String json = "{\"ok\":true,\"q\":" + String(q) + ",\"ms\":" + String(ms) + "}";
  server.send(200, "application/json", json);
}

void handleState() {
  String json = "{\"active\":" + String(activeQ) +
                ",\"pulse\":" + (pulseActive ? "true" : "false") +
                ",\"pulseQ\":" + String(pulseActive ? pulseQ : -1) + "}";
  server.send(200, "application/json", json);
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

// Redirect all unknown AP requests to captive portal
void handleNotFound() {
  if (apMode) {
    server.sendHeader("Location", "http://" + WiFi.softAPIP().toString() + "/");
    server.send(302);
  } else {
    server.send(404, "text/plain", "Not found");
  }
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

  // 595 pins
  pinMode(PIN_DATA,  OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_LATCH, OUTPUT);
  setOutput(-1);  // all off at boot

  // Load saved credentials
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

    // mDNS  → http://twitchblows.local
    if (MDNS.begin(HOSTNAME)) {
      MDNS.addService("http", "tcp", 80);
      DPRINTLN("mDNS: http://" HOSTNAME ".local");
    }

    setupOTA();
  }

  // Web routes
  server.on("/",          handleRoot);
  server.on("/set",       handleSet);
  server.on("/pulse",     handlePulse);
  server.on("/state",     handleState);
  server.on("/savewifi",  handleSaveWifi);
  server.onNotFound(      handleNotFound);
  server.begin();
  DPRINTLN("HTTP server started");
}

// ── Loop ──────────────────────────────────────
void loop() {
  if (!apMode) {
    ArduinoOTA.handle();

    // ── Pulse auto-off ──────────────────────
    if (pulseActive && (millis() >= pulseEnd)) {
      shiftWrite(0);          // drop the pin
      pulseActive = false;
      pulseQ      = -1;
      DPRINTLN("Pulse ended — output OFF");
    }

    // Reconnect STA if dropped
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
  } else {
    dns.processNextRequest();
  }

  server.handleClient();
}
