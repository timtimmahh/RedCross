/**
 * @dir main/devices/GPS/
 *
 * @file GPS.h
 *
 * @brief GPS class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_GPS_SRC_GPS_H
#define REDCROSS_LIB_GPS_SRC_GPS_H

#include "UARTPerif.hpp"
//#include <Adafruit_GPS.h>
#include <string>

namespace perif {

/**
 * A GPS peripheral. Tested on Adafruit's Ultimate GPS breakout.
 */
class GPS : public UARTPerif {
 private:
//  Adafruit_GPS gps;
  // the minimum amount of time to wait to update
  const uint8_t updateInterval;
  // the time of the last update
  // whether data has been updated or not
  bool updated = false;
  void updateData() override;
  void representData() override;
 public:
  /**
   * Explicit value constructor for a GPS sensor.
   *
   * @param updateInterval the update interval in seconds
   */
  explicit GPS(uint8_t updateInterval = 2);
  bool begin() override;
};
}
#endif //REDCROSS_LIB_GPS_SRC_GPS_H
