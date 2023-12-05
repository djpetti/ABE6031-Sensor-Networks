#include <Wire.h>
#include <EEPROM.h>

#include "DHT.h"

#include "air_quality.hpp"
#include "serial_comms.hpp"

// Pin that the DHT-11 sensor is connected to.
#define DHTPIN 7
// We are using a DHT-11.
#define DHTTYPE DHT11

// Pin that the light sensor is connected to.
#define LIGHT_SENSOR_PIN 0

DHT dht(DHTPIN, DHTTYPE);
AirQuality co2_sensor;

// How much time to leave between baseline calibrations for the air quality sensor, in seconds.
const uint8_t kBaselinePeriod = 30;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial console to open!

  dht.begin();
  co2_sensor.Begin();
}

/** 
 * @brief return absolute humidity [mg/m^3] with approximation formula
 * @param temperature [°C]
 * @param humidity [%RH]
 * @return The absolute humidity
 */
float getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    return 1000.0f * absoluteHumidity; // [mg/m^3]
}

/**
 * @brief Reads temperature and humidity data from the sensor.
 * @param[out] temp Will be set to the current temperature.
 * @param[out] humidity Will be set to the current (absolute) humidity.
 * @return True if it successfully read, false if it failed to communicate.
 */
bool readTemperatureHumidity(float *temp, float *humidity) {
  *temp = dht.readTemperature();
  *humidity = dht.readHumidity();

  if (isnan(*temp) || !isnan(*humidity)) {
    return false;
  }

  // Convert to absolute humidity.
  *humidity = getAbsoluteHumidity(*temp, *humidity);

  return true;
}

void loop() {
  delay(1000);

  // Read temperature/humidity data.
  float temp, humidity;
  readTemperatureHumidity(&temp, &humidity);
  WriteTempHumidity(temp, humidity);

  // Read air quality data.
  uint16_t voc, co2;
  co2_sensor.ReadAirQuality(humidity, &voc, &co2);
  WriteAirQuality(co2, voc, co2_sensor.IsCalibrated());

  // Read light data.
  const uint16_t kRawLightValue = analogRead(LIGHT_SENSOR_PIN);
  const float kLightIntensity = 1.0 - (static_cast<float>(kRawLightValue) / 1024.0);
  WriteLightIntensity(kLightIntensity);
}
