/**
 * @file AccelGyro.h
 *
 * @brief AccelGyro class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H
#define REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H

#include <chrono>
#include <thread>
#include "I2CPerif.hpp"
#include "MPU6050.h"

namespace perif {

#define US_DELAY    3150

/**
 * Accelerometer and gyroscope wrapper around MPU6050 library by Jeff Rowberg.
 */
class AccelGyro : public I2CPerif<> {
 private:
  MPU6050 mpu;
  int16_t data[6];
  unsigned long lastUpdate;
  /**
   * Gets both acceleration and rotation data. (Magnetometer WIP).
   */
  void updateData() override;
  void representData() override;
 public:
  /**
   * Constructor with sda and scl pins.
   *
   * @param sdaPin the sda pin
   * @param sclPin the scl pin
   */
  explicit AccelGyro(uint8_t address = 0x68);
  bool begin() override;
  /**
   * Obtains the acceleration data.
   *
   * @return acceleration coords (x, y, z)
   */
  const int16_t *getAcceleration();
  /**
   * Obtains the gyroscope rotation data.
   *
   * @return rotation coords (x, y, z)
   */
  const int16_t *getRotation();
};
} // namespace perif
#endif //REDCROSS_LIB_ACCELGYRO_SRC_ACCELGYRO_H
