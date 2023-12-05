#ifndef SERIAL_COMMS_H_
#define SERIAL_COMMS_H_

#include <stdint.h>

// Implements the wire protocol for serial communication.

/**
 * @brief Writes the temperature and humidity to the serial.
 * @param temperature The temperature value, in C.
 * @param humidity The absolute humidity value, in mg/m^3.
 */
void WriteTempHumidity(float temperature, float humidity);

/**
 * @brief Writes the air quality info to the serial.
 * @param co2 The CO2 level, in PPM.
 * @param voc The VOC level, in PPB
 * @param calibrated Whether the sensor is currently calibrated.
 */
void WriteAirQuality(uint16_t co2, uint16_t voc, bool calibrated);

/**
 * @brief Writes the light intensity to the serial.
 * @param intensity The light intensity.
 */
void WriteLightIntensity(float intensity);

#endif  // SERIAL_COMMS_H_