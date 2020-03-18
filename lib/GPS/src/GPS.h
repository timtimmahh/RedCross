//
// Created by tim on 2/19/20.
//

#ifndef REDCROSS_LIB_GPS_SRC_GPS_H
#define REDCROSS_LIB_GPS_SRC_GPS_H

#include "peripheral/UARTSensor.hpp"
#include <Adafruit_GPS.h>
#include <string>

namespace perif {

/*class GPSData {
public:
  bool updated = false;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t seconds = 0;
  uint16_t milliSeconds = 0;
  uint8_t day = 0;
  uint8_t month = 0;
  uint8_t year = 0;
  bool hasFix = false;
  uint8_t fixQuality = 0;
  float latitude = 0.0;
  char lat = 'X';
  float longitude = 0.0;
  char lon = 'X';
  float speed = 0.0;
  float angle = 0.0;
  float altitude = 0.0;
  uint8_t satellites = 0;
  GPSData() = default;
  GPSData(const GPSData &original) = default;
};*/

/**
 * A GPS peripheral. Tested on Adafruit's Ultimate GPS breakout.
 */
class GPS : public UARTSensor {
 private:
  Adafruit_GPS gps;
  // the minimum amount of time to wait to update
  const uint8_t updateInterval;
  // the time of the last update
  uint8_t lastUpdate = 0;
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
