// I2Cdev library collection - Main I2C device class
// Abstracts bit and byte I2C R/W functions into a convenient class
// EFM32 stub port by Nicolas Baldeck <nicolas@pioupiou.fr>
// Based on Arduino's I2Cdev by Jeff Rowberg <jeff@rowberg.net>
//
// Changelog:
//      2015-01-02 - Initial release


/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2015 Jeff Rowberg, Nicolas Baldeck

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

#include "ErrorLog.hpp"
#include "i2c_data.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"

#include "I2Cdev.h"

/**
 * Default constructor.
 */
I2Cdev::I2Cdev(const i2c_port_t port)
	: port((static_cast<unsigned>(port) | 1u) == 1u ? port : I2C_NUM_0),
	  initialized(false) {
  i2c::portRefs[this->port]++;
}

I2Cdev::~I2Cdev() {
  i2c::portRefs[port]--;
  if (i2c::portRefs[port] == 0) {
	i2c::initializedPorts[port] = false;
	i2c::configs[port] = {};
	i2c_driver_delete(port);
  }
}

/**
 * Initialize I2C0.
 */
bool I2Cdev::initialize(gpio_num_t sda, gpio_num_t scl,
						bool sdaPullUp, bool sclPullUp, uint32_t frequency) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  ErrorHandler<> check("I2Cdev");
  if (i2c::initializedPorts[port]) {
	if (sda != i2c::configs[port].sda_io_num
		|| scl != i2c::configs[port].scl_io_num
		|| sdaPullUp != i2c::configs[port].sda_pullup_en
		|| sclPullUp != i2c::configs[port].scl_pullup_en) {
	  i2c::configs[port].sda_io_num = sda;
	  i2c::configs[port].scl_io_num = scl;
	  i2c::configs[port].sda_pullup_en = sdaPullUp ? GPIO_PULLUP_ENABLE
												   : GPIO_PULLUP_DISABLE;
	  i2c::configs[port].scl_pullup_en = sclPullUp ? GPIO_PULLUP_ENABLE
												   : GPIO_PULLUP_DISABLE;
	  initialized = check(i2c_set_pin, "I2C set pin", port,
						  static_cast<int>(i2c::configs[port].sda_io_num),
						  static_cast<int>(i2c::configs[port].scl_io_num),
						  i2c::configs[port].sda_pullup_en,
						  i2c::configs[port].scl_pullup_en,
						  I2C_MODE_MASTER).runCheck();
	} else initialized = true;
  } else {
	i2c_config_t &config = i2c::configs[port];
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = static_cast<gpio_num_t>(sda);
	config.sda_pullup_en = sdaPullUp ? GPIO_PULLUP_ENABLE
									 : GPIO_PULLUP_DISABLE;
	config.scl_io_num = static_cast<gpio_num_t>(scl);
	config.scl_pullup_en = sclPullUp ? GPIO_PULLUP_ENABLE
									 : GPIO_PULLUP_DISABLE;
	config.master.clk_speed = frequency;
	const i2c_config_t *conf = &config;
	initialized =
		check(i2c_param_config, "I2C Param Config", port, conf)
			(i2c_driver_install, "I2C Driver Install",
			 port, I2C_MODE_MASTER, static_cast<size_t>(0),
			 static_cast<size_t>(0), 0).runCheck();
  }
  i2c::initializedPorts[port] = initialized;
  return initialized;
}

/** Read a single bit from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitNum Bit position to read (0-7)
 * @param data Container for single bit value
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBit(uint8_t devAddr,
					   uint8_t regAddr,
					   uint8_t bitNum,
					   uint8_t *data,
					   uint16_t timeout) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  uint8_t b;
  uint8_t count = readByte_(devAddr, regAddr, &b, timeout);
  *data = b & (1u << bitNum);
  return count;
}

/** Read multiple bits from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param bitStart First bit position to read (0-7)
 * @param length Number of bits to read (not more than 8)
 * @param data Container for right-aligned value (i.e. '101' read from any bitStart position will equal 0x05)
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readBits(uint8_t devAddr,
						uint8_t regAddr,
						uint8_t bitStart,
						uint8_t length,
						uint8_t *data,
						uint16_t timeout) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  // 01101001 read byte
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  //    010   masked
  //   -> 010 shifted
  uint8_t count, b;
  if ((count = readByte_(devAddr, regAddr, &b, timeout)) != 0) {
	uint8_t mask = static_cast<uint8_t>((1u << length) - 1)
		<< static_cast<uint8_t>(bitStart - length + 1);
	b &= mask;
	b >>= static_cast<uint8_t>(bitStart - length + 1);
	*data = b;
  }
  return count;
}
int8_t I2Cdev::readByte_(uint8_t devAddr,
						 uint8_t regAddr,
						 uint8_t *data,
						 uint16_t timeout) {
  return readBytes_(devAddr, regAddr, 1, data, timeout);
}
/** Read single byte from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to read from
 * @param data Container for byte value read from device
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return Status of read operation (true = success)
 */
int8_t I2Cdev::readByte(uint8_t devAddr,
						uint8_t regAddr,
						uint8_t *data,
						uint16_t timeout) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  return readBytes_(devAddr, regAddr, 1, data, timeout);
}

int8_t I2Cdev::readBytes_(uint8_t devAddr,
						  uint8_t regAddr,
						  uint8_t length,
						  uint8_t *data,
						  uint16_t timeout) {
  i2c_cmd_handle_t cmd;
  SelectRegister(devAddr, regAddr);
  ErrorHandler<> check("I2Cdev");

  cmd = i2c_cmd_link_create();
  check(i2c_master_start, "I2C Start", cmd)
	  (i2c_master_write_byte, "I2C Write Byte", cmd,
	   static_cast<uint8_t>(static_cast<uint8_t>(
		   devAddr << 1u) | I2C_MASTER_READ
	   ), true);
//  ESP_ERROR_CHECK(i2c_master_start(cmd))
//  ESP_ERROR_CHECK(i2c_master_write_byte(cmd,
//										static_cast<uint8_t>(devAddr << 1u)
//											| I2C_MASTER_READ,
//										1))

  if (length > 1)
	check(i2c_master_read, "I2C Read", cmd, data,
		  static_cast<size_t>(length - 1),
		  I2C_MASTER_ACK);
//  ESP_ERROR_CHECK(i2c_master_read(cmd, data, length - 1, I2C_MASTER_ACK))

  check(i2c_master_read_byte, "I2C Read Byte",
		cmd, data + length - 1, I2C_MASTER_NACK);
//  ESP_ERROR_CHECK(i2c_master_read_byte(cmd,
//									   data + length - 1,
//									   I2C_MASTER_NACK))

  check(i2c_master_stop, "I2C Stop", cmd)
	  (i2c_master_cmd_begin, "I2C CMD Begin", port, cmd,
	   1000 / portTICK_PERIOD_MS);
//  ESP_ERROR_CHECK(i2c_master_cmd_begin(port,
//									   cmd,
//									   1000 / portTICK_PERIOD_MS))
  check.runCheck();
  i2c_cmd_link_delete(cmd);

  return length;
}

/** Read multiple bytes from an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr First register regAddr to read from
 * @param length Number of bytes to read
 * @param data Buffer to store read data in
 * @param timeout Optional read timeout in milliseconds (0 to disable, leave off to use default class value in I2Cdev::readTimeout)
 * @return I2C_TransferReturn_TypeDef http://downloads.energymicro.com/documentation/doxygen/group__I2C.html
 */
int8_t I2Cdev::readBytes(uint8_t devAddr,
						 uint8_t regAddr,
						 uint8_t length,
						 uint8_t *data,
						 uint16_t timeout) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  return readBytes_(devAddr, regAddr, length, data, timeout);
}

/**
 * read word
 * @param devAddr
 * @param regAddr
 * @param data
 * @param timeout
 * @return
 */
int8_t I2Cdev::readWord(uint8_t devAddr,
						uint8_t regAddr,
						uint16_t *data,
						uint16_t timeout) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  uint8_t msb[2] = {0, 0};
  readBytes_(devAddr, regAddr, 2, msb);
  *data = (static_cast<uint16_t>(msb[0] << 8u) | msb[1]);
  return 0;
}

void I2Cdev::SelectRegister(uint8_t dev, uint8_t reg) {
  i2c_cmd_handle_t cmd;

  cmd = i2c_cmd_link_create();
  ErrorHandler<>("I2Cdev")
	  (i2c_master_start, "I2C Start", cmd)
	  (i2c_master_write_byte, "I2C Write Byte", cmd,
	   static_cast<uint8_t>(static_cast<uint8_t>(dev << 1u)
		   | I2C_MASTER_WRITE),
	   true)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, reg, true)
	  (i2c_master_stop, "I2C Stop", cmd)
	  (i2c_master_cmd_begin, "I2C Begin", port, cmd,
	   1000 / portTICK_PERIOD_MS)
	  .runCheck();
  i2c_cmd_link_delete(cmd);
}

/** write a single bit in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitNum Bit position to write (0-7)
 * @param value New bit value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBit(uint8_t devAddr,
					  uint8_t regAddr,
					  uint8_t bitNum,
					  uint8_t data) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  uint8_t b;
  readByte_(devAddr, regAddr, &b);
  b = (data != 0) ? (b | (1u << bitNum)) : (b & ~(1u << bitNum));
  return writeByte_(devAddr, regAddr, b);
}

/** Write multiple bits in an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register regAddr to write to
 * @param bitStart First bit position to write (0-7)
 * @param length Number of bits to write (not more than 8)
 * @param data Right-aligned value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBits(uint8_t devAddr,
					   uint8_t regAddr,
					   uint8_t bitStart,
					   uint8_t length,
					   uint8_t data) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  //      010 value to write
  // 76543210 bit numbers
  //    xxx   args: bitStart=4, length=3
  // 00011100 mask byte
  // 10101111 original value (sample)
  // 10100011 original & ~mask
  // 10101011 masked | value
  uint8_t b = 0;
  if (readByte_(devAddr, regAddr, &b) != 0) {
	uint8_t mask = static_cast<uint8_t>((1u << length) - 1)
		<< static_cast<uint8_t>(bitStart - length + 1);
	data <<= static_cast<uint8_t>(bitStart - length + 1); // shift data into
	// correct position
	data &= mask; // zero all non-important bits in data
	b &= static_cast<uint8_t>(~(mask)); // zero all important bits in existing
	// byte
	b |= data; // combine data with existing byte
	return writeByte_(devAddr, regAddr, b);
  } else {
	return false;
  }
}

bool I2Cdev::writeByte_(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
  i2c_cmd_handle_t cmd;

  cmd = i2c_cmd_link_create();
  ErrorHandler<>("I2Cdev")
	  (i2c_master_start, "I2C Start", cmd)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, static_cast<uint8_t>(
		   static_cast<uint8_t>(devAddr << 1u) | I2C_MASTER_WRITE),
	   true)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, regAddr, true)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, data, true)
	  (i2c_master_stop, "I2C Stop", cmd)
	  (i2c_master_cmd_begin, "I2C Begin", port, cmd, 1000 / portTICK_PERIOD_MS)
	  .runCheck();
  i2c_cmd_link_delete(cmd);

  return true;
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param data New byte value to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  return writeByte_(devAddr, regAddr, data);
}

bool I2Cdev::writeBytes_(uint8_t devAddr,
						 uint8_t regAddr,
						 uint8_t length,
						 uint8_t *data) {
  i2c_cmd_handle_t cmd;

  cmd = i2c_cmd_link_create();
  ErrorHandler<>("I2Cdev")
	  (i2c_master_start, "I2C Start", cmd)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, static_cast<uint8_t>(
		  static_cast<uint8_t>(devAddr << 1u) | I2C_MASTER_WRITE), true)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, regAddr, true)
	  (i2c_master_write, "I2C Write", cmd, data,
	   static_cast<size_t>(length - 1), false)
	  (i2c_master_write_byte, "I2C Write Byte", cmd, data[length - 1], true)
	  (i2c_master_stop, "I2C Stop", cmd)
	  (i2c_master_cmd_begin, "I2C CMD Begin",
	   port, cmd, 1000 / portTICK_PERIOD_MS)
	  .runCheck();
  i2c_cmd_link_delete(cmd);
  return true;
}

/** Write single byte to an 8-bit device register.
 * @param devAddr I2C slave device address
 * @param regAddr Register address to write to
 * @param length Number of bytes to write
 * @param data Array of bytes to write
 * @return Status of operation (true = success)
 */
bool I2Cdev::writeBytes(uint8_t devAddr,
						uint8_t regAddr,
						uint8_t length,
						uint8_t *data) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  return writeBytes_(devAddr, regAddr, length, data);
}

bool I2Cdev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
  lock_guard<mutex> lock(i2c::portLocks[port]);
  uint8_t data1[] = {(uint8_t)(data >> 8u), (uint8_t)(data & 0xffu)};
  uint8_t data2[] = {(uint8_t)(data & 0xffu), (uint8_t)(data >> 8u)};
  writeBytes_(devAddr, regAddr, 2, data1);
  writeBytes_(devAddr, regAddr, 2, data2);
  return true;
}


