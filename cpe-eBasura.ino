#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "webUI.h"
#include "webJson.h"

WEBUI    web;
WEBJSON  webjson;

AsyncWebServer server(80);

const char* WIFI_SSID     = "CPEeBasura";
const char* WIFI_PASSWORD = "cpeebasura123";

#define TCA_ADDRESS    0x70
#define NUM_BINS       16
#define BIN_DEPTH_MM   400
#define FULL_THRESHOLD 90

// ── Bin data struct ──────────────────────────
struct BinData {
  String  name;
  float   fillPercent;
  int     distanceMM;
  bool    isFull;
  bool    sensorOK;
  float   mapX;
  float   mapY;
};

BinData bins[NUM_BINS];

// ── Mock fill for Zone B only (indices 8–15) ─
const int MOCK_FILL_B[8] = {
  88, 15, 95, 41,
  73, 30, 60, 50
};

void applyMockDataZoneB();
void initBins();

// ── Track last Zone A update ─────────────────
unsigned long lastZoneAUpdate = 0;
const unsigned long ZONE_A_TIMEOUT_MS = 10000; // 10 s

// ======================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  initBins();
  applyMockDataZoneB();   // Zone B starts with mock data

  // ── WiFi ─────────────────────────────────────
  Serial.printf("\nConnecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! http://%s\n",
    WiFi.localIP().toString().c_str());

  // ── Routes ───────────────────────────────────

  // Dashboard
  server.on("/", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", web.buildHTML());
    }
  );

  // JSON API for the browser dashboard
  server.on("/api/bins", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      req->send(200, "application/json",
        webjson.buildJSON(bins, NUM_BINS, FULL_THRESHOLD));
    }
  );

  // ── POST /api/zone/a ─────────────────────────
  // Zone A node POSTs: {"bins":[{"id":0,"dist":123},...]}
AsyncCallbackJsonWebHandler* zoneAHandler =
  new AsyncCallbackJsonWebHandler("/api/zone/a",
    [](AsyncWebServerRequest* req, JsonVariant& json) {

      JsonObject root = json.as<JsonObject>();
      JsonArray  arr  = root["bins"].as<JsonArray>();

      if (arr.isNull()) {
        req->send(400, "application/json",
          "{\"error\":\"missing bins array\"}");
        return;
      }

      // ── Log received update ─────────────────
      Serial.println("─────────────────────────────────");
      Serial.println("[SERVER] Received Zone A update:");

      for (JsonObject b : arr) {
        int id = b["id"] | -1;
        if (id < 0 || id > 7) continue;

        int distMM = b["dist"] | BIN_DEPTH_MM;
        distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

        float fill = 100.0f *
          (1.0f - (float)distMM / BIN_DEPTH_MM);
        fill = constrain(fill, 0.0f, 100.0f);

        bins[id].distanceMM  = distMM;
        bins[id].fillPercent = fill;
        bins[id].isFull      = fill >= FULL_THRESHOLD;
        bins[id].sensorOK    = b["ok"] | true;

        Serial.printf("  [Zone A] %s (id=%d) | dist=%4d mm | fill=%3.0f%% | %s\n",
          bins[id].name.c_str(),
          id,
          bins[id].distanceMM,
          bins[id].fillPercent,
          bins[id].isFull ? "FULL" : "OK"
        );
      }

      lastZoneAUpdate = millis();
      req->send(200, "application/json", "{\"status\":\"ok\"}");
    }
  );    new AsyncCallbackJsonWebHandler("/api/zone/a",
      [](AsyncWebServerRequest* req, JsonVariant& json) {

        JsonObject root = json.as<JsonObject>();
        JsonArray  arr  = root["bins"].as<JsonArray>();

        if (arr.isNull()) {
          req->send(400, "application/json",
            "{\"error\":\"missing bins array\"}");
          return;
        }

        for (JsonObject b : arr) {
          int id = b["id"] | -1;
          if (id < 0 || id > 7) continue; // Zone A = 0–7 only

          int distMM = b["dist"] | BIN_DEPTH_MM;
          distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

          float fill = 100.0f *
            (1.0f - (float)distMM / BIN_DEPTH_MM);
          fill = constrain(fill, 0.0f, 100.0f);

          bins[id].distanceMM  = distMM;
          bins[id].fillPercent = fill;
          bins[id].isFull      = fill >= FULL_THRESHOLD;
          bins[id].sensorOK    = b["ok"] | true;
        }

        lastZoneAUpdate = millis();
        req->send(200, "application/json", "{\"status\":\"ok\"}");

      }
    );

  server.addHandler(zoneAHandler);
  server.begin();
  Serial.println("HTTP server started.");
}

// ======================================================
void loop() {
  // If Zone A node hasn't reported in ZONE_A_TIMEOUT_MS,
  // mark Zone A bins offline so the dashboard shows it.
  if (millis() - lastZoneAUpdate > ZONE_A_TIMEOUT_MS
      && lastZoneAUpdate != 0) {
    for (int i = 0; i < 8; i++) {
      bins[i].sensorOK = false;
    }
  }

  applyMockDataZoneB();
  delay(2000);
}

// ======================================================
void initBins() {
  const char* names[NUM_BINS] = {
    "Bin A1","Bin A2","Bin A3","Bin A4",
    "Bin A5","Bin A6","Bin A7","Bin A8",
    "Bin B1","Bin B2","Bin B3","Bin B4",
    "Bin B5","Bin B6","Bin B7","Bin B8"
  };

  for (int i = 0; i < NUM_BINS; i++) {
    bins[i].name        = names[i];
    bins[i].fillPercent = 0;
    bins[i].distanceMM  = BIN_DEPTH_MM;
    bins[i].isFull      = false;
    bins[i].sensorOK    = false;
    bins[i].mapX        = 8.0f + (i % 8) * 12.0f;
    bins[i].mapY        = (i < 8) ? 28.0f : 68.0f;
  }
}

// ======================================================
void applyMockDataZoneB() {
  for (int i = 0; i < 8; i++) {
    int idx = i + 8; // Bins B1–B8
    bins[idx].fillPercent = MOCK_FILL_B[i];
    bins[idx].distanceMM  = (int)(BIN_DEPTH_MM *
      (1.0f - MOCK_FILL_B[i] / 100.0f));
    bins[idx].isFull      = MOCK_FILL_B[i] >= FULL_THRESHOLD;
    bins[idx].sensorOK    = true;
  }
}