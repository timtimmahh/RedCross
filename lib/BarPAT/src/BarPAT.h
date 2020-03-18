/**
 * @file BarPat.h
 *
 * @brief BarPAT class declaration.
 *
 * This work complies with the JMU honor code.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_BARPAT_SRC_BARPAT_H_
#define REDCROSS_LIB_BARPAT_SRC_BARPAT_H_

#include "peripheral/I2CSensor.hpp"
#include <Adafruit_MPL115A2.h>

namespace perif {

/**
 * A barometric pressure/temperature sensor.
 * TODO add altitude for upgraded mpl3115A2 sensor
 */
class BarPAT : public I2CSensor0 {
 private:
  Adafruit_MPL115A2 mpl;
  // the barometric pressure
  float pressure;
  // the barometric temperature
  float temp;
  void updateData() override;
  void representData() override;
 public:
  /**
   * Explicit value constructor for MPL115A2 sensor.
   *
   * @param address the I2C address
   */
  explicit BarPAT(uint8_t address = 0x60);
  bool begin() override;
};

}

#endif // REDCROSS_LIB_BARPAT_SRC_BARPAT_H_
