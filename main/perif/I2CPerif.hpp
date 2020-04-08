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

#define F_CPU (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000U)

#define PORT0_SDA GPIO_NUM_21
#define PORT0_SCL GPIO_NUM_22
#define PORT1_SDA GPIO_NUM_33
#define PORT1_SCL GPIO_NUM_32

/**
 * A peripheral connected over the I2C bus. The ESP32 has 2 I2C bus's and
 * only bus 0 has default sda and scl pins, however both can have the sda and
 * scl pins specified manually.
 */
template<i2c_port_t port = I2C_NUM_0,
	bool sdaPullup = false,
	bool sclPullup = false,
	uint32_t frequency = 100000UL>
class I2CPerif : public Perif, protected I2Cdev {
 protected:
  // the address for this i2c device
  uint8_t address;

 public:

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
 * @param wire_num the i2c bus number (ESP32 only has 2)
 */
  explicit I2CPerif(const char *name,
					uint8_t address)
	  : Perif(name), I2Cdev(port),
		address(address) {
  }

  bool isConnected() override {
// if i2c hasn't been initialized or failed, the device can't be used yet
	return initialized;
  }
};

}

#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
