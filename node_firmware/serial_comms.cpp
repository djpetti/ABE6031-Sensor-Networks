#include "serial_comms.hpp"

#include <ArduinoJson.h>

// The unique ID of this node.
#define NODE_ID 1

namespace {

StaticJsonDocument<58> temp_document;
StaticJsonDocument<84> air_quality_document;
StaticJsonDocument<58> light_document;

}  // namespace

void WriteTempHumidity(float temperature, float humidity) {
  temp_document[F("type")] = F("temp");
  temp_document[F("node")] = NODE_ID;

  temp_document[F("temp")] = temperature;
  temp_document[F("humidity")] = humidity;

  serializeJson(temp_document, Serial);
  Serial.println();
}

void WriteAirQuality(uint16_t co2, uint16_t voc, bool calibrated) {
  air_quality_document[F("type")] = F("air_qual");
  air_quality_document[F("node")] = NODE_ID;

  air_quality_document[F("co2")] = co2;
  air_quality_document[F("voc")] = voc;
  air_quality_document[F("calibrated")] = calibrated;

  serializeJson(air_quality_document, Serial);
  Serial.println();
}

void WriteLightIntensity(float intensity) {
  light_document[F("type")] = F("light");
  light_document[F("node")] = NODE_ID;

  light_document[F("intensity")] = intensity;

  serializeJson(light_document, Serial);
  Serial.println();
}