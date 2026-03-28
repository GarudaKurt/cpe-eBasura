#include <Arduino.h>
#include <ArduinoJson.h>

#include "webJson.h"

struct BinData {
  String  name;
  float   fillPercent;
  int     distanceMM;
  bool    isFull;
  bool    sensorOK;
  float   mapX;
  float   mapY;
};


WEBJSON::WEBJSON() {}


// ======================================================
// Build JSON
// ======================================================

String WEBJSON::buildJSON(
        BinData bins[],
        int numBins,
        int threshold
) {

  JsonDocument doc;

  JsonArray arr =
    doc["bins"].to<JsonArray>();


  for (int i = 0; i < numBins; i++) {

    JsonObject b =
      arr.add<JsonObject>();

    b["id"]   = i;
    b["name"] = bins[i].name;
    b["fill"] = (int)bins[i].fillPercent;
    b["dist"] = bins[i].distanceMM;
    b["full"] = bins[i].isFull;
    b["ok"]   = bins[i].sensorOK;
    b["x"]    = bins[i].mapX;
    b["y"]    = bins[i].mapY;

  }

  doc["timestamp"] = millis();
  doc["threshold"] = threshold;

  String out;

  serializeJson(doc, out);

  return out;
}