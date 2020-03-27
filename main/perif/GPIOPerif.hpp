/**
 * @file GPIOPerif.hpp
 *
 * @brief GPIOPerif class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H
#define REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H

#include <functional>
#include <array>
#include <driver/gpio.h>
#include "Perif.hpp"

namespace perif {

class GPIOConfig {

};

/**
 * A peripheral that interfaces with GPIO pins directly.
 */
class GPIOPerif : public Perif {
 private:
  using gpioFunc = function<void(uint8_t)>;
  static array<gpioFunc, GPIO_PIN_COUNT> isrHandlers;
  static void isrHandler(void *arg) {
	uint32_t pin = reinterpret_cast<long>(arg);
	isrHandlers[pin](pin);
  };
  static inline bool _setPin(uint8_t pin,
							 gpio_mode_t mode = GPIO_MODE_INPUT_OUTPUT,
							 bool pullUp = false,
							 bool pullDown = false,
							 gpio_int_type_t intrType = GPIO_INTR_DISABLE) {

	bool res;
	if ((mode & GPIO_MODE_OUTPUT) != GPIO_MODE_OUTPUT)
	  res = GPIO_IS_VALID_OUTPUT_GPIO(pin);
	else res = GPIO_IS_VALID_GPIO(pin);
	gpio_config_t gpio_cfg = {
		.pin_bit_mask = (1ULL << pin),
		.mode = mode,
		.pull_up_en = pullUp ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE,
		.pull_down_en = pullDown ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE,
		.intr_type = intrType
	};
	return res && gpio_config(&gpio_cfg) == ESP_OK;
  }
 public:
  /**
   * Explicit value constructor for GPIO peripherals.
   *
   * @param name the name of the peripheral
   */
  explicit GPIOPerif(const char *name) : Perif(name) {}
  bool begin() override {
	// TODO
	return true;
  }
  bool isConnected() override {
	// no way to tell if GPIO is connected or not
	return true;
  }
  bool setPin(uint8_t pin,
			  gpio_mode_t mode = GPIO_MODE_INPUT_OUTPUT,
			  bool pullUp = false,
			  bool pullDown = false) {
	return _setPin(pin, mode, pullUp, pullDown);
  }
  bool setIntrPin(uint8_t pin,
				  gpioFunc &&handler,
				  uint8_t isrFlags = 0,
				  gpio_int_type_t intrType = GPIO_INTR_ANYEDGE,
				  gpio_mode_t mode = GPIO_MODE_INPUT_OUTPUT,
				  bool pullUp = false,
				  bool pullDown = false) {
	if (intrType >= GPIO_INTR_MAX)
	  intrType = GPIO_INTR_ANYEDGE;
	if (_setPin(pin, mode, pullUp, pullDown, intrType)) {
	  esp_err_t err = gpio_install_isr_service(isrFlags);
	  if (err == ESP_OK || err == ESP_ERR_INVALID_STATE) {
		isrHandlers[pin] = move(handler);
		return gpio_isr_handler_add(static_cast<gpio_num_t>(pin),
									isrHandler,
									reinterpret_cast<void *>(pin)) == ESP_OK;
	  }
	}
	return false;
  }
};
}
#endif //REDCROSS_LIB_SENSOR_SRC_PERIPHERAL_PWMSENSOR_H
