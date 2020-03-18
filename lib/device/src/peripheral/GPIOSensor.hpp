//
// Created by tim on 2/25/20.
//

#ifndef REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H
#define REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H

#include "Perif.hpp"
namespace perif {
/**
 * A peripheral that interfaces with GPIO pins directly.
 */
class GPIOSensor : public Perif {
 public:
  /**
   * Explicit value constructor for GPIO peripherals.
   *
   * @param name the name of the peripheral
   */
  explicit GPIOSensor(const char *name) : Perif(name) {}
  bool begin() override {
    // TODO
	return true;
  }
  bool isConnected() override {
    // TODO
	return true;
  }
};
}
#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H
