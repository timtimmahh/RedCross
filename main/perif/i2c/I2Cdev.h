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

/**
 * @dir main/perif/i2c/
 *
 * @file I2Cdev.h
 *
 * @brief I2C device driver.
 */
#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#include <mutex>
#include <driver/gpio.h>
#include <driver/i2c.h>

/**
 * Default timeout value for read operations.
 */
#define I2CDEV_DEFAULT_READ_TIMEOUT 1000

using namespace std;

/**
 * The I2C bus operator.
 */
class I2Cdev {
 protected:
  /**
   * The I2C port for this device.
   */
  const i2c_port_t port;
  /**
   * Whether the device is initialized.
   */
  bool initialized;
  /**
   * Initializes the I2C port if not already and verifies this device is there.
   *
   * @param sda the SDA pin
   * @param scl the SCL pin
   * @param sdaPullUp whether SDA is a pullup
   * @param sclPullUp whether SCL is a pullup
   * @param frequency the bus frequency
   * @return whether initialization was successfull
   */
  bool initialize(gpio_num_t sda, gpio_num_t scl,
				  bool sdaPullUp = false, bool sclPullUp = false,
				  uint32_t frequency = 100000UL);
  /**
   * Reads a single byte.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the number of bytes read.
   */
  int8_t readByte_(uint8_t devAddr, uint8_t regAddr, uint8_t *data,
				   uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Reads multiple bytes.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param length the number of bytes to read
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the number of bytes read.
   */
  int8_t readBytes_(uint8_t devAddr, uint8_t regAddr,
					uint8_t length, uint8_t *data,
					uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Writes a single byte.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeByte_(uint8_t devAddr, uint8_t regAddr, uint8_t data);
  /**
   * Writes multiple bytes.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param length the number of bytes to write
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeBytes_(uint8_t devAddr, uint8_t regAddr,
				   uint8_t length, uint8_t *data);
 public:
  /**
   * Creates an instance to an I2C device on the specified port.
   *
   * @param port the I2C port
   */
  explicit I2Cdev(const i2c_port_t port);
  /**
   * Performs I2C bus cleanup on this port, if necessary.
   */
  ~I2Cdev();
  /**
   * Selects a device register.
   *
   * @param dev the device
   * @param reg the register
   */
  void SelectRegister(uint8_t dev, uint8_t reg);

  /**
   * Reads a single bit with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param bitNum the bit number in the register
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the length of data read
   */
  int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum,
				 uint8_t *data, uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Reads multiple bits with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param bitStart the register bit to start
   * @param length the number of bytes to read
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the length of data read
   */
  int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart,
				  uint8_t length, uint8_t *data,
				  uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Reads a single byte with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the number of bytes read.
   */
  int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data,
				  uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Reads a single word with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the number of bytes read.
   */
  int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data,
				  uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Reads multiple words with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param length the number of bytes to read
   * @param data the destination to read into
   * @param timeout read timeout
   * @return the number of bytes read.
   */
  int8_t readBytes(uint8_t devAddr, uint8_t regAddr,
				   uint8_t length, uint8_t *data,
				   uint16_t timeout = I2CDEV_DEFAULT_READ_TIMEOUT);
  /**
   * Writes a single bit with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param bitNum the register bit to write
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
  /**
   * Writes multiple bits with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param bitStart the register bit to start
   * @param length the number of bytes to write
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart,
				 uint8_t length, uint8_t data);
  /**
   * Writes a single byte with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
  /**
   * Writes multiple bytes with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param length the number of bytes to write
   * @param data the data to write
   * @return if write succeeded
   */
  bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length,
				  uint8_t *data);
  /**
   * Writes a single word with a mutex lock for thread-safety.
   *
   * @param devAddr the device address
   * @param regAddr the register address
   * @param data the destination to read into
   * @return if write succeeded.
   */
  bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);

  //TODO static int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
  //TODO static int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
  //TODO static int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout);
  //TODO static bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
  //TODO static bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
  //TODO static bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);
};

#endif /* _I2CDEV_H_ */
