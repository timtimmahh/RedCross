/**
 * @file UARTSensor.hpp
 *
 * @brief UARTSensor class declaration.
 *
 * This work complies with the JMU honor code.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_
#define REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_

#include <HardwareSerial.h>
#include "Perif.hpp"

#define DEFAULT_BAUD        9600

namespace perif {

/**
 * A peripheral that operates over serial UART. ESP32 has 3 UART bus's, one
 * being the serial used for logging. Most devices using UART on the ESP32 will
 * probably use UART2.
 */
class UARTSensor : public Perif {
 protected:
  // the baud rate
  int baud;
  // the transmit and receive GPIO pins
  uint8_t txPin, rxPin;
  // a reference to an existing HardwareSerial instance
  HardwareSerial &MySerial;
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
  explicit UARTSensor(const char *name,
					  uint8_t uart_num,
					  uint8_t txPin,
					  uint8_t rxPin,
					  int baud = DEFAULT_BAUD)
	  : Perif(name),
		baud(baud),
		txPin(txPin), rxPin(rxPin),
		MySerial(uart_num == 0 ? Serial : (uart_num == 1 ? Serial1 : Serial2)) {
  }
  /**
   * Explicit value constructor for a preconfigured HardwareSerial instance.
   *
   * @param name the name of the peripheral
   * @param serial a reference to the UART bus
   */
  explicit UARTSensor(const char *name, HardwareSerial &serial)
	  : Perif(name),
		baud(serial.baudRate()),
		txPin(-1), rxPin(-1),
		MySerial(serial) {

  }
  bool begin() override {
	MySerial.begin(baud, SERIAL_8N1, rxPin, txPin);
	return isConnected();
  }
  bool isConnected() override {
	return MySerial.available() || MySerial.availableForWrite();
  }
};

}

#endif //REDCROSS_LIB_DEVICE_SRC_PERIPHERAL_UARTSENSOR_HPP_
