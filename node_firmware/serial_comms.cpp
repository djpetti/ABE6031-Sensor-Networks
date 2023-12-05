#include "serial_comms.hpp"

#include <string.h>
#include <stdio.h>

#include <ArduinoJson.h>

// The unique ID of this node.
#define NODE_ID 1
// Maximum length of a message we can receive.
#define MAX_INPUT_MESSAGE_LENGTH 40
// Maximum length for a log message.
#define MAX_LOG_MESSAGE_LENGTH 150

namespace {

// Buffer used to store input messages.
char input_message_buffer[MAX_INPUT_MESSAGE_LENGTH];
// Buffer used to store log messages.
char log_message_buffer[MAX_LOG_MESSAGE_LENGTH];

}  // namespace

void WriteTempHumidity(float temperature, float humidity) {
  StaticJsonDocument<58> temp_document;
  temp_document[F("type")] = F("temp");
  temp_document[F("node")] = NODE_ID;

  temp_document[F("temp")] = temperature;
  temp_document[F("humidity")] = humidity;

  serializeJson(temp_document, Serial);
  Serial.println();
}

void WriteAirQuality(uint16_t co2, uint16_t voc, bool calibrated) {
  StaticJsonDocument<84> air_quality_document;
  air_quality_document[F("type")] = F("air_qual");
  air_quality_document[F("node")] = NODE_ID;

  air_quality_document[F("co2")] = co2;
  air_quality_document[F("voc")] = voc;
  air_quality_document[F("calibrated")] = calibrated;

  serializeJson(air_quality_document, Serial);
  Serial.println();
}

void WriteLightIntensity(float intensity) {
  StaticJsonDocument<58> light_document;
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
  StaticJsonDocument<58> input_document;
  const DeserializationError kError = deserializeJson(input_document, input_message_buffer);
  if (kError) {
    // Failed to parse.
    Log(F("Invalid JSON message, ignoring."));
    return InputCommandType::UNKNOWN;
  }

  // Check the command type.
  if (!strcmp(input_document[F("type")], "start_calibration")) {
    return InputCommandType::START_CALIBRATION;
  }
  Log(F("Unknown message type, ignoring."));
  return InputCommandType::UNKNOWN;
}

void Log(const __FlashStringHelper* format, ...) {
  va_list args;
  va_start(args, format);
  vsnprintf_P(log_message_buffer, MAX_LOG_MESSAGE_LENGTH, reinterpret_cast<const char PROGMEM*>(format), args);
  va_end(args);

  StaticJsonDocument<200> log_document;
  log_document[F("type")] = F("log");
  log_document[F("node")] = NODE_ID;
  log_document[F("message")] = log_message_buffer;

  serializeJson(log_document, Serial);
  Serial.println();
}