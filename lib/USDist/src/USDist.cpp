//
// Created by tim on 2/25/20.
//

#include "USDist.h"

namespace perif {

USDist::USDist(uint8_t trigPin, uint8_t echoPin)
	: GPIOSensor("USDist"),
	  trigPin(trigPin),
	  echoPin(echoPin),
	  lastMeasure(0),
	  distance(0.0) {
}

bool USDist::begin() {
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  return GPIOSensor::begin();
}

void USDist::updateData() {

  unsigned long t = millis();
  if (t - lastMeasure < 60)
	delay(60 - (t - lastMeasure));
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  distance = static_cast<float>(pulseIn(echoPin, HIGH)) * SOUND_SPEED;
  lastMeasure = millis();
}

void USDist::representData() {
  data["distance"] = to_string(distance, 2);
}

// an implementation using esp-idf APIs
/*
#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000
#define ROUNDTRIP 58

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

#define check_timeout(start, len) if ((esp_timer_get_time() - (start)) >= (len)) \
	RETURN_CRITICAL()

#define CHECK(x) x == ESP_OK
#define NCHECK(x) if (x != ESP_OK) RETURN_CRITICAL()
#define RETURN_CRITICAL(RES) { \
	portEXIT_CRITICAL(&mux); \
	return RES; \
}
#define TRIG_PIN    GPIO_NUM_17
#define ECHO_PIN    GPIO_NUM_16
bool USDist::begin() {
  return CHECK(gpio_set_direction(TRIG_PIN, GPIO_MODE_OUTPUT))
	  && CHECK(gpio_set_direction(ECHO_PIN, GPIO_MODE_INPUT))
	  && CHECK(gpio_set_level(TRIG_PIN, 0));
}
void USDist::updateData() {
  portENTER_CRITICAL(&mux);
  NCHECK(gpio_set_level(TRIG_PIN, 0))
  ets_delay_us(TRIGGER_LOW_DELAY);
  NCHECK(gpio_set_level(TRIG_PIN, 1))
  ets_delay_us(TRIGGER_HIGH_DELAY);
  NCHECK(gpio_set_level(TRIG_PIN, 0))

  if (gpio_get_level(ECHO_PIN))
	RETURN_CRITICAL()

  int64_t start = esp_timer_get_time();
  while (!gpio_get_level(ECHO_PIN))
    check_timeout(start, PING_TIMEOUT)
  int64_t echo_start = esp_timer_get_time();
  int64_t time = echo_start;
  int64_t meas_timeout = echo_start + 400 * ROUNDTRIP;

  while (gpio_get_level(ECHO_PIN)) {
    time = esp_timer_get_time();
    check_timeout(echo_start, meas_timeout)
  }
  portEXIT_CRITICAL(&mux);
  distance = static_cast<float>(time - echo_start) / ROUNDTRIP;
}
*/
}