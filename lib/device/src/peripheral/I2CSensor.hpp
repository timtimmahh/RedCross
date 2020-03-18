//
// Created by tim on 2/25/20.
//

#ifndef REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
#define REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H

#include "Perif.hpp"
#include <Wire.h>

namespace perif {

/**
 * Maintains initialization information about the i2c bus.
 */
struct i2cInfo {
  // the sda pin
  int8_t sda;
  // the scl pin
  int8_t scl;
  // whether the i2c bus has been initialized
  bool began;
};

static i2cInfo i2cStarted[2] = {
    {-1, -1, false},
    {-1, -1, false}
};

/**
 * A peripheral connected over the I2C bus. The ESP32 has 2 I2C bus's and
 * only bus 0 has default sda and scl pins, however both can have the sda and
 * scl pins specified manually.
 *
 * @tparam sda the sda pin for this I2C bus (-1 uses the default pin)
 * @tparam scl the scl pin for this I2C bus (-1 uses the default pin)
 */
template<int sda, int scl>
class I2CSensor : public Perif {
 protected:
  // the address for this i2c device
  uint8_t address;
  // the i2c bus number
  uint8_t wire_num;
  // a reference to the TwoWire instance for the i2c bus
  TwoWire &wire;

  bool begin() override {
	// initialize i2c bus with the specified pins
	if (i2cStarted[wire_num].sda != sda
		|| i2cStarted[wire_num].scl != scl) {
	  // if pins are different than before, the wire needs to be reinitialized
	  // warning in case of mistakenly using the same TwoWire instance
	  if (i2cStarted[wire_num].began)
        Serial.println("Warning: restarting I2C with different pins. Devices "
                       "connected to the old pins will no longer work.");
	  i2cStarted[wire_num].sda = sda;
	  i2cStarted[wire_num].scl = scl;
	  i2cStarted[wire_num].began = false;
	}
	// if the wire already began, there's no need to start it again
	return i2cStarted[wire_num].began
		|| (i2cStarted[wire_num].began = wire.begin(sda, scl));
  }
 public:
  /**
   * Explicit value constructor for an I2C peripheral.
   *
   * @param name the name of the peripheral
   * @param address the i2c address
   * @param wire_num the i2c bus number (ESP32 only has 2)
   */
  explicit I2CSensor(const char *name, uint8_t address, uint8_t num = 0)
	  : Perif(name),
		address(address),
		wire_num(num & 0x1),
		wire(wire_num == 0 ? Wire : Wire1) {}

  bool isConnected() override {
    // if i2c hasn't been initialized or failed, the device can't be used yet
	if (!i2cStarted[wire_num].began)
	  return false;
	// perform a dummy transmission to see if it succeeds
	wire.beginTransmission(address);
	return wire.endTransmission(true) == 0;
  }
};

using I2CSensor0 = I2CSensor<-1, -1>;

}

#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_I2CSENSOR_H
