//
// Created by tim on 2/9/20.
//

#ifndef REDCROSS_LIB_LIDARMAP_H
#define REDCROSS_LIB_LIDARMAP_H

#include <Sensor.h>
#include <LIDARLite.h>

/**
 * Wrapper around the Garmin LIDAR-Lite library.
 */
class LidarMap : public Sensor<double> {
private:
  LIDARLite lidar;

  /**
   * Gets the distance (in cm) count times and returns the average.
   */
  double getDistance(int count = 1);
public:
  /**
   * Constructor for Lidar. Sets up Lidar immediately.
   */
  LidarMap() : lidar() {
    lidar.reset();
    delay(1000);
    lidar.begin();
  }
  /**
   * The name of this sensor.
   *
   * @return the sensor name
   */
  String name() override;
  /**
   * Obtains distance data in centimeters.
   *
   * @return distance in centimeters
   */
  double getData() override;
  /**
   * Converts distance data to a String
   *
   * @param data distance data
   * @return String representation of distance
   */
  String dataAsString(const double &data) override;
};

#endif //REDCROSS_LIB_LIDARMAP_H
