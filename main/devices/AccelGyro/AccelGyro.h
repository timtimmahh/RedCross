/**
 * @dir main/devices/AccelGyro/
 *
 * @file AccelGyro.h
 *
 * @brief Accelerometer and gyroscope wrapper around MPU6050 library by Jeff Rowberg.
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

class AccelGyro : public I2CPerif<> {
 private:
  /**
   * MPU6050 instance.
   */
  MPU6050 mpu;
  /**
   * Accelerometer and gyroscope data.
   * Acceleration = [0, 1, 2]
   * Gyroscope = [3, 4, 5]
   */
  int16_t data[6];
  /**
   * Most recent data update. (prevents updating too soon)
   */
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
   * @param address the I2C device address
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
