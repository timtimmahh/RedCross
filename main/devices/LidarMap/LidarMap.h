/**
 * @file LidarMap.h
 *
 * @brief LidarMap class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_LIDARMAP_H
#define REDCROSS_LIB_LIDARMAP_H

#include "I2CPerif.hpp"
#include <cstdint>

#define conf(v1, v2, v3) configure<v1, v2, v3>()
#define conf0 conf(0x80, 0x08, 0x00)
#define conf1 conf(0x1d, 0x08, 0x00)
#define conf2 conf(0x80, 0x00, 0x00)
#define conf3 conf(0xff, 0x08, 0x00)
#define conf4 conf(0x80, 0x08, 0x80)
#define conf5 conf(0x80, 0x08, 0xb0)

#define doConfig() conf0

namespace perif {
/**
 * Wrapper around the Garmin LIDAR-Lite library.
 */
class LidarMap : public I2CPerif {
 private:
  template<int v1, int v2, int v3>
  bool configure();
  // the distance in cm
  double distance;
 protected:
  bool begin() override;
 private:
  void reset();
  /**
   * Obtains distance data in centimeters.
   */
  void updateData() override;
  /**
   * Converts distance data to a String.
   */
  void representData() override;
 public:
  /**
   * Constructor for Lidar. Sets up Lidar immediately.
   */
  explicit LidarMap(uint8_t address = 0x62);
  /**
   * Gets the distance (in cm) count times and returns the average.
   */
  double getDistance(uint8_t count = 1);
  double measure(bool biasCorrection);
};
}
#endif //REDCROSS_LIB_LIDARMAP_H
