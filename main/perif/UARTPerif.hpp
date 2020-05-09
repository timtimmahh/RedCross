/**
 * @dir main/perif/
 *
 * @file UARTPerif.hpp
 *
 * @brief UARTPerif class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_
#define REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_

#include "Perif.hpp"

/**
 * Default baud rate
 */
#define DEFAULT_BAUD        9600

/**
 * @namespace perif
 *
 * @brief The peripheral library.
 */
namespace perif {

/**
 * A peripheral that operates over serial UART. ESP32 has 3 UART bus's, one
 * being the serial used for logging. Most devices using UART on the ESP32 will
 * probably use UART2.
 */
class UARTPerif : public Perif {
 protected:
  /**
   * The baud rate.
   */
  int baud;
  /**
   * The transmit GPIO pins.
   */
  uint8_t txPin;
  /**
   * The receive GPIO pins.
   */
  uint8_t rxPin;
 public:
  /**
   * Explicit value constructor for UART devices.
   *
   * @param name the name of the peripheral
   * @param uart_num the UART bus number
   * @param txPin the GPIO transmit pin
   * @param rxPin the GPIO receive pin
   * @param baud the baud rate
   */
  explicit UARTPerif(const char *name,
					 uint8_t uart_num,
					 uint8_t txPin,
					 uint8_t rxPin,
					 int baud = DEFAULT_BAUD)
	  : Perif(name),
		baud(baud),
		txPin(txPin), rxPin(rxPin) {
//		MySerial(uart_num == 0 ? Serial : (uart_num == 1 ? Serial1 : Serial2)) {
  }
  /**
   * Explicit value constructor for a preconfigured HardwareSerial instance.
   *
   * @param name the name of the peripheral
   * @param baud the baud rate
   */
  explicit UARTPerif(const char *name, int baud = DEFAULT_BAUD)
	  : Perif(name),
		baud(baud),
		txPin(-1), rxPin(-1) {

  }
  bool begin() override {
//	MySerial.begin(baud, SERIAL_8N1, rxPin, txPin);
	return isConnected();
  }
  bool isConnected() override {
//	return MySerial.available() || MySerial.availableForWrite();
	return false;
  }
};

}

#endif //REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_
