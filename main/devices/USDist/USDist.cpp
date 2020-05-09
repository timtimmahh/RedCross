/**
 * @dir main/devices/USDist/
 *
 * @file USDist.cpp
 *
 * @brief USDist class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "USDist.h"

namespace perif {

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP 58

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

#define check_timeout(len) if (timer.elapsedTime() >= (len)) \
    RETURN_CRITICAL()

#define CHECK(x) x == ESP_OK
#define NCHECK(x) if (x != ESP_OK) RETURN_CRITICAL()
#define RETURN_CRITICAL(RES) { \
    portEXIT_CRITICAL(&mux); \
    return RES; \
}
USDist::USDist(uint8_t trigPin, uint8_t echoPin)
	: GPIOPerif("USDist"),
	  trigPin(trigPin),
	  echoPin(echoPin),
	  distance(0.0) {
}

bool USDist::begin() {
  setPin(trigPin, GPIO_MODE_OUTPUT);
  setPin(echoPin, GPIO_MODE_INPUT);
  gpio_set_level(static_cast<gpio_num_t>(trigPin), 0);
  return GPIOPerif::begin();
}

/*static unsigned long millis() {
  return duration_cast<milliseconds>(system_clock::now()
										 .time_since_epoch()).count();
}*/

void USDist::updateData() {
//  unsigned long t = millis();
//  if (t - lastMeasure < 60)
//    std::this_thread::sleep_for(milliseconds{60 - (t - lastMeasure)});
//  gpio_set_level(static_cast<gpio_num_t>(trigPin), 1);
//  std::this_thread::sleep_for(microseconds{10});
//  gpio_set_level(static_cast<gpio_num_t>(trigPin), 0);
//
//  distance = static_cast<float>(pulseIn(echoPin, HIGH)) * SOUND_SPEED;
//  lastMeasure = millis();
  portENTER_CRITICAL(&mux);
  NCHECK(gpio_set_level(TRIG_PIN, 0))
  ets_delay_us(TRIGGER_LOW_DELAY);
  NCHECK(gpio_set_level(TRIG_PIN, 1))
  ets_delay_us(TRIGGER_HIGH_DELAY);
  NCHECK(gpio_set_level(TRIG_PIN, 0))

  if (gpio_get_level(ECHO_PIN)) RETURN_CRITICAL()

  timer.startTimeLapse();
  while (!gpio_get_level(ECHO_PIN))
	check_timeout(PING_TIMEOUT)
  timer.startTimeLapse();
  uint64_t start = timer.getLatestUpdate();
  uint64_t time = timer.getLatestUpdate();
  uint64_t meas_timeout = time + 400 * ROUNDTRIP;

  while (gpio_get_level(ECHO_PIN)) {
	time = timer.getLatestUpdate();
	check_timeout(meas_timeout)
  }
  portEXIT_CRITICAL(&mux);
  distance = static_cast<float>(time - start) / ROUNDTRIP;
}

void USDist::representData() {
  data["distance"] = to_string(distance, 2);
}

// an implementation using esp-idf APIs

/*bool USDist::begin() {
  return CHECK(gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT))
	  && CHECK(gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT))
	  && CHECK(gpio_set_level(TRIG_PIN, 0));
}
void USDist::updateData() {

}*/

}