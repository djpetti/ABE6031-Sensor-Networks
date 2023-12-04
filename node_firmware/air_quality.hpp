#ifndef AIR_QUALITY_HPP_
#define AIR_QUALITY_HPP_

#include "Adafruit_SGP30.h"

/**
 * @brief Class for handling the air quality sensor.
 */
class AirQuality {
 public:
  /**
   * @brief Initializes the sensor.
   * @return True if it succeeds, false otherwise.
   */
  bool Begin();

  /**
    * @brief Reads the data from the air quality sensor.
    * @param humidity The ambient absolute humidity, in mg/m^3.
    * @param[out] voc Will be set to the VOC levels, in ppb.
    * @param[out] co2 Will be set to the eCO2 levels, in ppm.
    * @return True if it successfully read, false if it failed to communicate.
    */
  bool ReadAirQuality(float humidity, uint16_t *voc, uint16_t *co2);

  /**
   * @return True if the sensor is currently calibrated.
   */
  inline bool IsCalibrated() {
    return is_calibrated_;
  }

  /**
   * @brief Forces the sensor to start a new calibration.
   */
  void StartCalibration();

 private:
  /**
   * @brief Attempts to load the baseline from EEPROM. If it succeeds, it will
   * write it to the sensor.
   * @return True if it successfully read the baseline, false if there there is no
   *  stored baseline.
   */
  bool SetBaseline();
  /**
   * @brief Reads the current baseline from the sensor and saves it to EEPROM.
   */
  void SaveBaseline(); 

  /// The sensor to read from.
  Adafruit_SGP30 sgp_;

  /// Time at which we last read the calibration values.
  uint32_t calibration_time_ = 0;
  /// Whether or not the sensor is calibrated.
  bool is_calibrated_ = false;
};

#endif  // AIR_QUALITY_HPP_