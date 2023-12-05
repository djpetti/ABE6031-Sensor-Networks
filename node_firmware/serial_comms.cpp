#include "serial_comms.hpp"

#include <string.h>

#include <ArduinoJson.h>

// The unique ID of this node.
#define NODE_ID 1
// Maximum length of a message we can receive.
#define MAX_INPUT_MESSAGE_LENGTH 40

namespace {

// Buffer used to store input messages.
char input_message_buffer[MAX_INPUT_MESSAGE_LENGTH];

StaticJsonDocument<58> temp_document;
StaticJsonDocument<84> air_quality_document;
StaticJsonDocument<58> light_document;
StaticJsonDocument<58> input_document;

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

InputCommandType HandleIncomingMessage() {
  // Read the message from the serial.
  char current_char;
  uint16_t write_index = 0;
  do {
    current_char = Serial.read();
    input_message_buffer[write_index++] = current_char;
  } while (current_char != "\n" && write_index < MAX_INPUT_MESSAGE_LENGTH);

  // Parse the JSON.
  const DeserializationError kError = deserializeJson(input_document, input_message_buffer);
  if (kError) {
    // Failed to parse.
    return InputCommandType::UNKNOWN;
  }

  // Check the command type.
  if (!strcmp(input_document[F("type")], "start_calibration")) {
    return InputCommandType::START_CALIBRATION;
  }
  return InputCommandType::UNKNOWN;
}