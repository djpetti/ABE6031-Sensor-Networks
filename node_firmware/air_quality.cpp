#include <EEPROM.h>

#include "air_quality.hpp"
#include "serial_comms.hpp"

// Starting address in the EEPROM of the VOC baseline value.
#define VOC_BASELINE_ADDR 0x00
// Starting address in the EEPROM of the CO2 baseline value.
#define CO2_BASELINE_ADDR 0x02
// Starting address in the EEPROM of the value indicating whether the baselines are valid.
#define BASELINES_GOOD_ADDR 0x03

// How long we have to wait, in seconds, to calibrate the baseline.
#define INITIAL_CALIBRATION_TIME 12 * 60 * 60
// Interval at which we save the baseline to EEPROM, in seconds.
#define BASELINE_SAVE_PERIOD 60 * 60

namespace {

/**
 * @brief Reads 16 bits from the EEPROM.
 * @param address The starting address.
 * @return The value it read.
 */
uint16_t ReadEeprom16(uint32_t address) {
  return static_cast<uint16_t>(EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

/**
 * @brief Writes 16 bits from the EEPROM.
 * @param address The starting address.
 * @param value The value to write.
 */
void WriteEeprom16(uint32_t address, uint16_t value) {
  EEPROM.update(address, value >> 8);
  EEPROM.update(address + 1, value);
}

}  // namespace

/**
 * @brief Reads a 16-bit value from EEPROM.
 * @param address The starting address.
 * @return The value that it read.
 */ 

bool AirQuality::Begin() {
  if (!sgp_.begin()) {
    Log(F("Failed to initialize SGP-30."));
    return false;
  }

  SetBaseline();
  return true;
}

bool AirQuality::ReadAirQuality(float humidity, uint16_t *voc, uint16_t *co2) {
  // Check the calibration status.
  const uint32_t kCalibrationElapsedTime = millis() / 1000 - calibration_time_;
  if (!is_calibrated_) {
    if (kCalibrationElapsedTime >= INITIAL_CALIBRATION_TIME) {
      // We have waited long enough. We can save the calibration and proceed.
      Log(F("Saving initial baseline."));
      SaveBaseline();
    }
  } else if (kCalibrationElapsedTime >= BASELINE_SAVE_PERIOD) {
    // We have to update the baseline again.
    SaveBaseline();
  }

  // Configure humidity compensation.
  sgp_.setHumidity(static_cast<uint32_t>(humidity));

  if (!sgp_.IAQmeasure()) {
    // Failed to measure anything.
    Log(F("Failed to read from the SGP-30."));
    return false;
  }

  *voc = sgp_.TVOC;
  *co2 = sgp_.eCO2;

  return true;
}

bool AirQuality::SetBaseline() {
  // Try reading the baseline from EEPROM.
  const bool kBaselineValid = EEPROM.read(BASELINES_GOOD_ADDR);
  if (!kBaselineValid) {
    // It has never been calibrated.
    Log(F("No valid baseline in EEPROM, not setting."));
    return false;
  }

  const uint16_t kVocBaseline = ReadEeprom16(VOC_BASELINE_ADDR);
  const uint16_t kCo2Baseline = ReadEeprom16(CO2_BASELINE_ADDR);

  Log(F("Setting new baseline: %u, %u"), kCo2Baseline, kVocBaseline);
  sgp_.setIAQBaseline(kCo2Baseline, kVocBaseline);
  is_calibrated_ = true;

  return true;
}

void AirQuality::SaveBaseline() {
  // Read the baseline from the sensor.
  uint16_t co2_baseline, voc_baseline;
  sgp_.getIAQBaseline(&co2_baseline, &voc_baseline);

  // Save it to EEPROM.
  Log(F("Saving baseline: %u, %u"), co2_baseline, voc_baseline);
  WriteEeprom16(VOC_BASELINE_ADDR, voc_baseline);
  WriteEeprom16(CO2_BASELINE_ADDR, co2_baseline);
  EEPROM.update(BASELINES_GOOD_ADDR, 1);

  calibration_time_ = millis();
}

void AirQuality::StartCalibration() {
  Log(F("Starting calibration."));

  // Mark the stored calibration as not valid in case it restarts before
  // finishing the calibration.
  EEPROM.update(BASELINES_GOOD_ADDR, 0);

  is_calibrated_ = false;
}