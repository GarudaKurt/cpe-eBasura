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

const char* WIFI_SSID     = "KurtTyler";
const char* WIFI_PASSWORD = "Pandemaca0";

#define NUM_BINS       8     // 4x Zone A + 4x Zone B
#define BIN_DEPTH_MM   400
#define FULL_THRESHOLD 90

// ── Bin data struct ──────────────────────────────
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

void initBins();

// ── Schedule data ────────────────────────────────
struct ScheduleData {
  int mwf[NUM_BINS];
  int mwfCount;
  int tth[NUM_BINS];
  int tthCount;
  int fs[NUM_BINS];
  int fsCount;
  int commercials[NUM_BINS];
  int commercialsCount;
};

ScheduleData sched;

void initSchedule() {
  sched.mwfCount         = 0;
  sched.tthCount         = 0;
  sched.fsCount          = 0;
  sched.commercialsCount = 0;
  memset(sched.mwf,         -1, sizeof(sched.mwf));
  memset(sched.tth,         -1, sizeof(sched.tth));
  memset(sched.fs,          -1, sizeof(sched.fs));
  memset(sched.commercials, -1, sizeof(sched.commercials));
}

// ── Track last zone updates ──────────────────────
unsigned long lastZoneAUpdate = 0;
unsigned long lastZoneBUpdate = 0;
const unsigned long ZONE_TIMEOUT_MS = 10000;


int distToPercent(int distMm) {
  if (distMm <= 50)  return 100;   // < 50mm  → 100%
  if (distMm <= 100) return 50;    // < 100mm → 50%
  if (distMm > 170)  return 10;    // > 170mm → 10%
  // 100–170mm → interpolate between 50% and 10%
  return map(distMm, 100, 170, 50, 10);
}

// ======================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();

  initBins();
  initSchedule();

  // ── WiFi ─────────────────────────────────────
  Serial.printf("\nConnecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! http://%s\n",
    WiFi.localIP().toString().c_str());

  // ── GET / — Dashboard HTML ────────────────────
  server.on("/", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      req->send(200, "text/html", web.buildHTML());
    }
  );

  // ── GET /api/bins — Live bin data ─────────────
  server.on("/api/bins", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      req->send(200, "application/json",
        webjson.buildJSON(bins, NUM_BINS, FULL_THRESHOLD));
    }
  );

  // ── GET /api/schedule ─────────────────────────
  server.on("/api/schedule", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      JsonDocument doc;

      JsonArray mwfArr = doc["mwf"].to<JsonArray>();
      for (int i = 0; i < sched.mwfCount; i++) mwfArr.add(sched.mwf[i]);

      JsonArray tthArr = doc["tth"].to<JsonArray>();
      for (int i = 0; i < sched.tthCount; i++) tthArr.add(sched.tth[i]);

      JsonArray fsArr = doc["fs"].to<JsonArray>();
      for (int i = 0; i < sched.fsCount; i++) fsArr.add(sched.fs[i]);

      JsonArray commArr = doc["commercials"].to<JsonArray>();
      for (int i = 0; i < sched.commercialsCount; i++)
        commArr.add(sched.commercials[i]);

      String out;
      serializeJson(doc, out);
      req->send(200, "application/json", out);
    }
  );

  // ── POST /api/schedule ────────────────────────
  AsyncCallbackJsonWebHandler* schedHandler =
    new AsyncCallbackJsonWebHandler("/api/schedule",
      [](AsyncWebServerRequest* req, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();

        auto parseDay = [&](const char* key, int* arr, int& count) {
          count = 0;
          JsonArray a = root[key].as<JsonArray>();
          if (a.isNull()) return;
          for (JsonVariant v : a) {
            int id = v.as<int>();
            if (id >= 0 && id < NUM_BINS && count < NUM_BINS)
              arr[count++] = id;
          }
        };

        parseDay("mwf", sched.mwf, sched.mwfCount);
        parseDay("tth", sched.tth, sched.tthCount);
        parseDay("fs",  sched.fs,  sched.fsCount);
        parseDay("commercials", sched.commercials, sched.commercialsCount);

        Serial.println("─────────────────────────────────");
        Serial.println("[SERVER] Schedule updated:");
        Serial.printf("  MWF  (%d): ", sched.mwfCount);
        for (int i = 0; i < sched.mwfCount; i++) Serial.printf("%d ", sched.mwf[i]);
        Serial.println();
        Serial.printf("  TTH  (%d): ", sched.tthCount);
        for (int i = 0; i < sched.tthCount; i++) Serial.printf("%d ", sched.tth[i]);
        Serial.println();
        Serial.printf("  FS   (%d): ", sched.fsCount);
        for (int i = 0; i < sched.fsCount;  i++) Serial.printf("%d ", sched.fs[i]);
        Serial.println();
        Serial.printf("  COMM (%d): ", sched.commercialsCount);
        for (int i = 0; i < sched.commercialsCount; i++)
          Serial.printf("%d ", sched.commercials[i]);
        Serial.println();

        req->send(200, "application/json", "{\"status\":\"ok\"}");
      }
    );

  // ── POST /api/zone/a ──────────────────────────
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

        Serial.println("─────────────────────────────────");
        Serial.println("[SERVER] Zone A update:");

        for (JsonObject b : arr) {
          int id = b["id"] | -1;
          // Zone A uses ids 0–3
          if (id < 0 || id > 3) continue;

          int distMM = b["dist"] | BIN_DEPTH_MM;
          distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

          float fill = (float)distToPercent(distMM);
          bins[id].distanceMM  = distMM;
          bins[id].fillPercent = fill;
          bins[id].isFull      = fill >= FULL_THRESHOLD;
          bins[id].sensorOK    = b["ok"] | true;

          Serial.printf("  %s (id=%d) | dist=%4d mm | fill=%3.0f%% | %s\n",
            bins[id].name.c_str(), id,
            bins[id].distanceMM,
            bins[id].fillPercent,
            bins[id].isFull ? "FULL" : "OK");
        }

        lastZoneAUpdate = millis();
        req->send(200, "application/json", "{\"status\":\"ok\"}");
      }
    );

  // ── POST /api/zone/b ──────────────────────────
  AsyncCallbackJsonWebHandler* zoneBHandler =
    new AsyncCallbackJsonWebHandler("/api/zone/b",
      [](AsyncWebServerRequest* req, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();
        JsonArray  arr  = root["bins"].as<JsonArray>();

        if (arr.isNull()) {
          req->send(400, "application/json",
            "{\"error\":\"missing bins array\"}");
          return;
        }

        Serial.println("─────────────────────────────────");
        Serial.println("[SERVER] Zone B update:");

        for (JsonObject b : arr) {
          int id = b["id"] | -1;
          // Zone B uses ids 4–7
          if (id < 4 || id > 7) continue;

          int distMM = b["dist"] | BIN_DEPTH_MM;
          distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

          float fill = (float)distToPercent(distMM);
          
          bins[id].distanceMM  = distMM;
          bins[id].fillPercent = fill;
          bins[id].isFull      = fill >= FULL_THRESHOLD;
          bins[id].sensorOK    = b["ok"] | true;

          Serial.printf("  %s (id=%d) | dist=%4d mm | fill=%3.0f%% | %s\n",
            bins[id].name.c_str(), id,
            bins[id].distanceMM,
            bins[id].fillPercent,
            bins[id].isFull ? "FULL" : "OK");
        }

        lastZoneBUpdate = millis();
        req->send(200, "application/json", "{\"status\":\"ok\"}");
      }
    );

  server.addHandler(schedHandler);
  server.addHandler(zoneAHandler);
  server.addHandler(zoneBHandler);
  server.begin();
  Serial.println("HTTP server started.");
}

// ======================================================
void loop() {

  // Zone A timeout — mark A bins offline
  if (lastZoneAUpdate != 0 &&
      millis() - lastZoneAUpdate > ZONE_TIMEOUT_MS) {
    for (int i = 0; i <= 3; i++) bins[i].sensorOK = false;
  }

  // Zone B timeout — mark B bins offline
  if (lastZoneBUpdate != 0 &&
      millis() - lastZoneBUpdate > ZONE_TIMEOUT_MS) {
    for (int i = 4; i <= 7; i++) bins[i].sensorOK = false;
  }

  delay(2000);
}

// ======================================================
void initBins() {

  const char* names[NUM_BINS] = {
    "Bin A1", "Bin A2", "Bin A3", "Bin A4",
    "Bin B1", "Bin B2", "Bin B3", "Bin B4"
  };

  // Zone A — top row of map (viewBox 760x400)
  // Zone B — bottom row of map
  const float mapX[NUM_BINS] = { 20.0f, 40.0f, 60.0f, 80.0f,
                                  20.0f, 40.0f, 60.0f, 80.0f };
  const float mapY[NUM_BINS] = { 25.0f, 25.0f, 25.0f, 25.0f,
                                  75.0f, 75.0f, 75.0f, 75.0f };

  for (int i = 0; i < NUM_BINS; i++) {
    bins[i].name        = names[i];
    bins[i].fillPercent = 0;
    bins[i].distanceMM  = BIN_DEPTH_MM;
    bins[i].isFull      = false;
    bins[i].sensorOK    = false;
    bins[i].mapX        = mapX[i];
    bins[i].mapY        = mapY[i];
  }
}
