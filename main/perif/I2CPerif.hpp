/**
 * @dir main/perif/
 *
 * @file I2CPerif.hpp
 *
 * @brief I2CPerif class declaration.
 *
 * @namespace perif
 *
 * @brief The peripheral library.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
#define REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H

#include "i2c/I2Cdev.h"
#include "Perif.hpp"

namespace perif {

/**
 * Default SDA pin for bus 0
 */
#define PORT0_SDA GPIO_NUM_21
/**
 * Default SCL pin for bus 0
 */
#define PORT0_SCL GPIO_NUM_22
/**
 * Default SDA pin for bus 1
 */
#define PORT1_SDA GPIO_NUM_33
/**
 * Default SCL pin for bus 1
 */
#define PORT1_SCL GPIO_NUM_32

/**
 * * A peripheral connected over the I2C bus. The ESP32 has 2 I2C bus's and
 * only bus 0 has default sda and scl pins, however both can have the sda and
 * scl pins specified manually.
 *
 * @tparam port I2C bus port
 * @tparam sdaPullup if SDA pin is pullup
 * @tparam sclPullup if SCL pin is pullup
 * @tparam frequency I2C bus frequency
 */
template<i2c_port_t port = I2C_NUM_0,
	bool sdaPullup = false,
	bool sclPullup = false,
	uint32_t frequency = 100000UL>
class I2CPerif : public Perif, protected I2Cdev {
 protected:
  /**
   * the address for this i2c device
   */
  uint8_t address;
 public:
  /**
   * Initializes the I2C port.
   *
   * @return whether the port was initialized
   */
  bool begin() override {
	return initialize(port == I2C_NUM_0 ? PORT0_SDA : PORT1_SDA,
					  port == I2C_NUM_0 ? PORT0_SCL : PORT1_SCL,
					  sdaPullup, sclPullup, frequency);
  }
  /**
   * Explicit value constructor for an I2C peripheral.
   *
   * @param name the name of the peripheral
   * @param address the i2c address
   */
  explicit I2CPerif(const char *name,
					uint8_t address)
	  : Perif(name), I2Cdev(port),
		address(address) {
  }

  /**
   * Whether this I2C port is initialized.
   *
   * @return if the I2C port is initialized
   */
  bool isConnected() override {
// if i2c hasn't been initialized or failed, the device can't be used yet
	return initialized;
  }
};

}

#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
