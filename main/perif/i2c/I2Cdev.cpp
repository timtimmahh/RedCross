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

#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"

#include "I2Cdev.h"

#define I2C_NUM I2C_NUM_0

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); /*assert(0 && #x);*/} } while(0);

/**
 * Default constructor.
 */
I2Cdev::I2Cdev(i2c_port_t port)
	: initialized(false),
	  portLock(portLocks[port]),
	  port(port) {
  portRefs[port]++;
}

I2Cdev::~I2Cdev() {
  portRefs[port]--;
  if (portRefs[port] == 0) {
	initializedPorts[port] = false;
	configs[port] = {};
	i2c_driver_delete(port);
  }
}

/**
 * Initialize I2C0.
 */
bool I2Cdev::initialize(gpio_num_t sda, gpio_num_t scl,
						bool sdaPullUp, bool sclPullUp, uint32_t frequency) {
  lock_guard<mutex> lock(portLock);
  if (initializedPorts[port]) {
	if (sda != configs[port].sda_io_num
		|| scl != configs[port].scl_io_num
		|| sdaPullUp != configs[port].sda_pullup_en
		|| sclPullUp != configs[port].scl_pullup_en) {
	  configs[port].sda_io_num = sda;
	  configs[port].scl_io_num = scl;
	  configs[port].sda_pullup_en = sdaPullUp ? GPIO_PULLUP_ENABLE
											  : GPIO_PULLUP_DISABLE;
	  configs[port].scl_pullup_en = sclPullUp ? GPIO_PULLUP_ENABLE
											  : GPIO_PULLUP_DISABLE;
	  initialized = i2c_set_pin(port,
								configs[port].sda_io_num,
								configs[port].scl_io_num,
								configs[port].sda_pullup_en,
								configs[port].scl_pullup_en,
								I2C_MODE_MASTER) == ESP_OK;
	}
  } else {
	i2c_config_t &config = configs[port];
	config.mode = I2C_MODE_MASTER;
	config.sda_io_num = static_cast<gpio_num_t>(sda);
	config.sda_pullup_en = sdaPullUp ? GPIO_PULLUP_ENABLE
									 : GPIO_PULLUP_DISABLE;
	config.scl_io_num = static_cast<gpio_num_t>(scl);
	config.scl_pullup_en = sclPullUp ? GPIO_PULLUP_ENABLE
									 : GPIO_PULLUP_DISABLE;
	config.master.clk_speed = frequency;
	initialized = i2c_param_config(port, &configs[port]) == ESP_OK
		&& i2c_driver_install(port, configs[port].mode, 0, 0, 0) == ESP_OK;
  }
  initializedPorts[port] = initialized;
  return initialized;
}

/** Enable or disable I2C
 * @param isEnabled true = enable, false = disable
 */
//void I2Cdev::enable(bool isEnabled) {
//
//}
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
  lock_guard<mutex> lock(portLock);
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
  lock_guard<mutex> lock(portLock);
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
  lock_guard<mutex> lock(portLock);
  return readBytes_(devAddr, regAddr, 1, data, timeout);
}

int8_t I2Cdev::readBytes_(uint8_t devAddr,
						  uint8_t regAddr,
						  uint8_t length,
						  uint8_t *data,
						  uint16_t timeout) {
  i2c_cmd_handle_t cmd;
  SelectRegister(devAddr, regAddr);

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd,
										static_cast<uint8_t>(devAddr << 1u)
											| I2C_MASTER_READ,
										1))

  if (length > 1)
	ESP_ERROR_CHECK(i2c_master_read(cmd, data, length - 1, I2C_MASTER_ACK))

  ESP_ERROR_CHECK(i2c_master_read_byte(cmd,
									   data + length - 1,
									   I2C_MASTER_NACK))

  ESP_ERROR_CHECK(i2c_master_stop(cmd))
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM,
									   cmd,
									   1000 / portTICK_PERIOD_MS))
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
  lock_guard<mutex> lock(portLock);
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
  lock_guard<mutex> lock(portLock);
  uint8_t msb[2] = {0, 0};
  readBytes_(devAddr, regAddr, 2, msb);
  *data = (static_cast<uint16_t>(msb[0] << 8u) | msb[1]);
  return 0;
}

void I2Cdev::SelectRegister(uint8_t dev, uint8_t reg) {
  i2c_cmd_handle_t cmd;

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd,
										static_cast<uint8_t>(dev << 1u)
											| I2C_MASTER_WRITE, 1))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, reg, 1))
  ESP_ERROR_CHECK(i2c_master_stop(cmd))
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM,
									   cmd,
									   1000 / portTICK_PERIOD_MS))
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
  lock_guard<mutex> lock(portLock);
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
  lock_guard<mutex> lock(portLock);
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
  ESP_ERROR_CHECK(i2c_master_start(cmd))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd,
										static_cast<uint8_t>(devAddr << 1u)
											| I2C_MASTER_WRITE,
										1))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, regAddr, 1))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data, 1))
  ESP_ERROR_CHECK(i2c_master_stop(cmd))
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM,
									   cmd,
									   1000 / portTICK_PERIOD_MS))
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
  lock_guard<mutex> lock(portLock);
  return writeByte_(devAddr, regAddr, data);
}

bool I2Cdev::writeBytes_(uint8_t devAddr,
						 uint8_t regAddr,
						 uint8_t length,
						 uint8_t *data) {
  i2c_cmd_handle_t cmd;

  cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd,
										static_cast<uint8_t>(devAddr << 1u)
											| I2C_MASTER_WRITE,
										1))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, regAddr, 1))
  ESP_ERROR_CHECK(i2c_master_write(cmd, data, length - 1, 0))
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, data[length - 1], 1))
  ESP_ERROR_CHECK(i2c_master_stop(cmd))
  ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM,
									   cmd,
									   1000 / portTICK_PERIOD_MS))
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
  lock_guard<mutex> lock(portLock);
  return writeBytes_(devAddr, regAddr, length, data);
}

bool I2Cdev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
  lock_guard<mutex> lock(portLock);
  uint8_t data1[] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xff)};
  uint8_t data2[] = {(uint8_t)(data & 0xff), (uint8_t)(data >> 8)};
  writeBytes_(devAddr, regAddr, 2, data1);
  return true;
}


