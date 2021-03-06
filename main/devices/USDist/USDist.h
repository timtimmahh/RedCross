/**
 * @dir main/devices/USDist/
 *
 * @file USDist.h
 *
 * @brief USDist class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_USDISTS_USDIST_H
#define REDCROSS_LIB_USDISTS_USDIST_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <thread>
#include "GPIOPerif.hpp"
#include <driver/gpio.h>
#include <freertos/portmacro.h>

namespace perif {

// the speed of sound through air at sea-level / 2
#define SOUND_SPEED 0.01715f

// default trig and echo GPIO pins
//#define TRIG_PIN	17
//#define ECHO_PIN	16

#define TRIG_PIN    GPIO_NUM_17
#define ECHO_PIN    GPIO_NUM_16

/**
 * Ultrasonic distance sensor peripheral. Tested on HC-SR04
 */
class USDist : public GPIOPerif {
 private:
  // the GPIO trigger pin
  uint8_t trigPin;
  // the GPIO echo pin
  uint8_t echoPin;;
  // the latest distance measured (in cm)
  float distance;
  void updateData() override;
  void representData() override;
 public:
  /**
   * Explicit value constructor.
   *
   * @param trigPin the GPIO trigger pin
   * @param echoPin the GPIO echo pin
   */
  explicit USDist(uint8_t trigPin = TRIG_PIN, uint8_t echoPin = ECHO_PIN);
  bool begin() override;
};

}

#endif //REDCROSS_LIB_USDISTS_USDIST_H
