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
// Each array holds up to 16 bin IDs (-1 = unused slot)
struct ScheduleData {
  int mwf[NUM_BINS];
  int mwfCount;
  int tth[NUM_BINS];
  int tthCount;
  int fs[NUM_BINS];
  int fsCount;
};

ScheduleData sched;

void initSchedule() {
  sched.mwfCount = 0;
  sched.tthCount = 0;
  sched.fsCount  = 0;
  memset(sched.mwf, -1, sizeof(sched.mwf));
  memset(sched.tth, -1, sizeof(sched.tth));
  memset(sched.fs,  -1, sizeof(sched.fs));
}

// ── Track last zone updates ──────────────────────
unsigned long lastZoneAUpdate = 0;
unsigned long lastZoneBUpdate = 0;
const unsigned long ZONE_A_TIMEOUT_MS = 10000;
const unsigned long ZONE_B_TIMEOUT_MS = 10000;


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

  // ── GET /api/schedule — Return saved schedule ─
  server.on("/api/schedule", HTTP_GET,
    [](AsyncWebServerRequest* req) {
      JsonDocument doc;

      JsonArray mwfArr = doc["mwf"].to<JsonArray>();
      for (int i = 0; i < sched.mwfCount; i++) mwfArr.add(sched.mwf[i]);

      JsonArray tthArr = doc["tth"].to<JsonArray>();
      for (int i = 0; i < sched.tthCount; i++) tthArr.add(sched.tth[i]);

      JsonArray fsArr  = doc["fs"].to<JsonArray>();
      for (int i = 0; i < sched.fsCount;  i++) fsArr.add(sched.fs[i]);

      String out;
      serializeJson(doc, out);
      req->send(200, "application/json", out);
    }
  );

  // ── POST /api/schedule — Save schedule ────────
  // Body: {"mwf":[0,1,2],"tth":[3,4],"fs":[5,6]}
  AsyncCallbackJsonWebHandler* schedHandler =
    new AsyncCallbackJsonWebHandler("/api/schedule",
      [](AsyncWebServerRequest* req, JsonVariant& json) {
        JsonObject root = json.as<JsonObject>();

        // Helper lambda to parse one day array
        auto parseDay = [&](const char* key, int* arr, int& count) {
          count = 0;
          JsonArray a = root[key].as<JsonArray>();
          if (a.isNull()) return;
          for (JsonVariant v : a) {
            int id = v.as<int>();
            if (id >= 0 && id < NUM_BINS && count < NUM_BINS) {
              arr[count++] = id;
            }
          }
        };

        parseDay("mwf", sched.mwf, sched.mwfCount);
        parseDay("tth", sched.tth, sched.tthCount);
        parseDay("fs",  sched.fs,  sched.fsCount);

        Serial.println("─────────────────────────────────");
        Serial.println("[SERVER] Schedule updated:");
        Serial.printf("  MWF (%d bins): ", sched.mwfCount);
        for (int i = 0; i < sched.mwfCount; i++) Serial.printf("%d ", sched.mwf[i]);
        Serial.println();
        Serial.printf("  TTH (%d bins): ", sched.tthCount);
        for (int i = 0; i < sched.tthCount; i++) Serial.printf("%d ", sched.tth[i]);
        Serial.println();
        Serial.printf("  FS  (%d bins): ", sched.fsCount);
        for (int i = 0; i < sched.fsCount;  i++) Serial.printf("%d ",  sched.fs[i]);
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
        Serial.println("[SERVER] Received Zone A update:");

        for (JsonObject b : arr) {
          int id = b["id"] | -1;
          if (id < 0 || id > 7) continue;

          int distMM = b["dist"] | BIN_DEPTH_MM;
          distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

          float fill = 100.0f * (1.0f - (float)distMM / BIN_DEPTH_MM);
          fill = constrain(fill, 0.0f, 100.0f);

          bins[id].distanceMM  = distMM;
          bins[id].fillPercent = fill;
          bins[id].isFull      = fill >= FULL_THRESHOLD;
          bins[id].sensorOK    = b["ok"] | true;

          Serial.printf("  [Zone A] %s (id=%d) | dist=%4d mm | fill=%3.0f%% | %s\n",
            bins[id].name.c_str(), id, bins[id].distanceMM,
            bins[id].fillPercent, bins[id].isFull ? "FULL" : "OK");
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
        Serial.println("[SERVER] Received Zone B update:");

        for (JsonObject b : arr) {
          int id = b["id"] | -1;
          if (id < 8 || id > 15) continue;

          int distMM = b["dist"] | BIN_DEPTH_MM;
          distMM     = constrain(distMM, 0, BIN_DEPTH_MM);

          float fill = 100.0f * (1.0f - (float)distMM / BIN_DEPTH_MM);
          fill = constrain(fill, 0.0f, 100.0f);

          bins[id].distanceMM  = distMM;
          bins[id].fillPercent = fill;
          bins[id].isFull      = fill >= FULL_THRESHOLD;
          bins[id].sensorOK    = b["ok"] | true;

          Serial.printf("  [Zone B] %s (id=%d) | dist=%4d mm | fill=%3.0f%% | %s\n",
            bins[id].name.c_str(), id, bins[id].distanceMM,
            bins[id].fillPercent, bins[id].isFull ? "FULL" : "OK");
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
  if (millis() - lastZoneAUpdate > ZONE_A_TIMEOUT_MS
      && lastZoneAUpdate != 0) {
    for (int i = 0; i < 8; i++) bins[i].sensorOK = false;
  }

  if (millis() - lastZoneBUpdate > ZONE_B_TIMEOUT_MS
      && lastZoneBUpdate != 0) {
    for (int i = 8; i < 16; i++) bins[i].sensorOK = false;
  }

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