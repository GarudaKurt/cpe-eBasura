/*
 * ============================================================
 *  TrashBinMonitor.ino
 * ============================================================
 */

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "webUI.h"
#include "webJson.h"

// ── Objects ─────────────────────────────────

WEBUI web;
WEBJSON webjson;

AsyncWebServer server(80);

// ── Toggle mock data ────────────────────────
#define USE_MOCK_DATA true

const char* WIFI_SSID     = "cpesmartbmi";
const char* WIFI_PASSWORD = "cpesmartbmi";

// ── Hardware constants ──────────────────────
#define TCA_ADDRESS     0x70
#define NUM_BINS        16
#define BIN_DEPTH_MM    400
#define FULL_THRESHOLD  90

// ── Sensor objects ──────────────────────────
Adafruit_VL53L0X sensors[NUM_BINS];

// ── Bin data struct ─────────────────────────
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

// ── Mock fill values ────────────────────────
const int MOCK_FILL[NUM_BINS] = {

  12, 34, 78, 55,
  92, 23, 67, 45,

  88, 15, 95, 41,
  73, 30, 60, 50

};

// ── Function declarations ───────────────────

void tcaSelect(uint8_t channel);
void initBins();
void applyMockData();
void readAllSensors();
float distanceToFill(int distanceMM);



// ======================================================
void setup() {

  Serial.begin(115200);
  Wire.begin();

  initBins();

#if USE_MOCK_DATA

  Serial.println("*** MOCK DATA MODE — sensors skipped ***");

  applyMockData();

#else

  Serial.println("Initialising sensors...");

  for (uint8_t i = 0; i < NUM_BINS; i++) {

    tcaSelect(i);

    if (!sensors[i].begin()) {

      Serial.printf(
        "[WARN] Bin %02d sensor FAILED\n",
        i + 1
      );

      bins[i].sensorOK = false;

    }
    else {

      Serial.printf(
        "[OK] Bin %02d ready\n",
        i + 1
      );

      bins[i].sensorOK = true;

    }

  }

#endif



  // ── WiFi connect ─────────────────────

  Serial.printf(
    "\nConnecting to %s ",
    WIFI_SSID
  );

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");

  }

  Serial.printf(
    "\nConnected! Open http://%s\n",
    WiFi.localIP().toString().c_str()
  );



  // ======================================================
  // ROUTES
  // ======================================================

  // MAIN PAGE

  server.on("/",
    HTTP_GET,
    [](AsyncWebServerRequest* request) {

      request->send(
        200,
        "text/html",
        web.buildHTML()
      );

    }
  );



  // JSON API

  server.on("/api/bins",
    HTTP_GET,
    [](AsyncWebServerRequest* request) {

      request->send(
        200,
        "application/json",
        webjson.buildJSON(
          bins,
          NUM_BINS,
          FULL_THRESHOLD
        )
      );

    }
  );



  server.begin();

  Serial.println("HTTP server started.");

}



// ======================================================
void loop() {

#if USE_MOCK_DATA

  applyMockData();
  delay(2000);

#else

  readAllSensors();
  delay(2000);

#endif

}

// ======================================================
void tcaSelect(uint8_t channel) {

  if (channel > 7) return;

  Wire.beginTransmission(TCA_ADDRESS);
  Wire.write(1 << channel);
  Wire.endTransmission();

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

    bins[i].isFull   = false;
    bins[i].sensorOK = false;

    bins[i].mapX =
      8.0f + (i % 8) * 12.0f;

    bins[i].mapY =
      (i < 8) ? 28.0f : 68.0f;

  }

}

// ======================================================
void applyMockData() {

  for (int i = 0; i < NUM_BINS; i++) {

    bins[i].fillPercent =
      MOCK_FILL[i];

    bins[i].distanceMM =
      (int)(
        BIN_DEPTH_MM *
        (1.0f -
        MOCK_FILL[i] / 100.0f)
      );

    bins[i].isFull =
      MOCK_FILL[i] >= FULL_THRESHOLD;

    bins[i].sensorOK = true;

  }

}

// ======================================================
void readAllSensors() {

  for (uint8_t i = 0; i < NUM_BINS; i++) {

    if (!bins[i].sensorOK)
      continue;

    tcaSelect(i);

    VL53L0X_RangingMeasurementData_t measure;

    sensors[i].rangingTest(
      &measure,
      false
    );

    if (measure.RangeStatus != 4) {

      bins[i].distanceMM =
        measure.RangeMilliMeter;

      bins[i].fillPercent =
        distanceToFill(
          measure.RangeMilliMeter
        );

      bins[i].isFull =
        bins[i].fillPercent >=
        FULL_THRESHOLD;

    }

  }

}

// ======================================================
float distanceToFill(int distanceMM) {

  distanceMM =
    constrain(
      distanceMM,
      0,
      BIN_DEPTH_MM
    );

  float fill =
    100.0f *
    (1.0f -
    (float)distanceMM /
    BIN_DEPTH_MM);

  return constrain(
    fill,
    0.0f,
    100.0f
  );
}