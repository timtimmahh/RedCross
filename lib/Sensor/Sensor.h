//
// Created by tim on 2/6/20.
//

#ifndef REDCROSS_INCLUDE_SENSOR_H
#define REDCROSS_INCLUDE_SENSOR_H

#include "Arduino.h"

/**
 * Abstract class for sensors. Used to manage all sensor data and persistence.
 *
 * @tparam DataType the data type
 */
template <class DataType>
class Sensor {
public:
  /**
   * Default constructor.
   */
  Sensor() = default;
  /**
   * The name of the sensor.
   *
   * @return the sensor name
   */
  virtual String name() = 0;
  /**
   * Obtains the sensor data.
   *
   * @return the sensor data
   */
  virtual DataType getData() = 0;
  /**
   * Converts the sensor data to a String. (Useful for debugging)
   *
   * @param data the sensor data
   * @return sensor data as a String
   */
  virtual String dataAsString(const DataType& data) = 0;
  /**
   * Wrapper around the data for global configuration.
   *
   * @return sensor data
   */
  DataType getSensorData() {
    return getData();
  }
};

#endif //REDCROSS_INCLUDE_SENSOR_H
