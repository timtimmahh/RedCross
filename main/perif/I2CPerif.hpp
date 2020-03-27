/**
 * @file I2CPerif.hpp
 *
 * @brief I2CPerif class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
#define REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H

#include "i2c/I2Cdev.h"
#include "Perif.hpp"

namespace perif {

#define PORT0_SDA GPIO_NUM_21
#define PORT0_SCL GPIO_NUM_22
#define PORT1_SDA GPIO_NUM_33
#define PORT1_SCL GPIO_NUM_32

/**
 * A peripheral connected over the I2C bus. The ESP32 has 2 I2C bus's and
 * only bus 0 has default sda and scl pins, however both can have the sda and
 * scl pins specified manually.
 *
 * @tparam sda the sda pin for this I2C bus (-1 uses the default pin)
 * @tparam scl the scl pin for this I2C bus (-1 uses the default pin)
 */
class I2CPerif : public Perif, protected I2Cdev {
 protected:
  // the address for this i2c device
  uint8_t address;
  // a reference to the TwoWire instance for the i2c bus
//  TwoWire &wire;
 public:

  bool begin() override {
	return initialize(port == I2C_NUM_0 ? PORT0_SDA : PORT1_SDA,
					  port == I2C_NUM_0 ? PORT0_SCL : PORT1_SCL);
  }
/**
 * Explicit value constructor for an I2C peripheral.
 *
 * @param name the name of the peripheral
 * @param address the i2c address
 * @param wire_num the i2c bus number (ESP32 only has 2)
 */
  explicit I2CPerif(const char *name,
					uint8_t address,
					i2c_port_t port = I2C_NUM_0)
	  : Perif(name), I2Cdev(port),
		address(address) {
  }

  bool isConnected() override {
// if i2c hasn't been initialized or failed, the device can't be used yet
	return initializedPorts[port];
  }
};

}

#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
