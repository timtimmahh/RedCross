/**
 * @file Timer.hpp
 *
 * @brief Timer class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_IPC_TIMER_HPP_
#define REDCROSS_MAIN_IPC_TIMER_HPP_

#include <esp_timer.h>

class Timer {
 private:
  uint64_t latest;
  esp_timer_handle_t timer;
  static void timer_cb(void *args) {

  }

 public:
  Timer() : latest(0), timer() {
	esp_timer_create_args_t args = {
		.callback = timer_cb,
		.arg = &latest,
		.dispatch_method = ESP_TIMER_TASK,
		.name = "default",
	};

	if (esp_timer_create(&args, &timer) == ESP_OK)
	  latest = esp_timer_get_time();
  }

  virtual void start() {

  }

  void startTimeLapse() {
	latest = esp_timer_get_time();
  }

  uint64_t getLatestUpdate() {
	return latest;
  }

  uint64_t getRuntime() {
	return esp_timer_get_time();
  }

  uint64_t elapsedTime() {
	uint64_t oldStart = latest;
	latest = esp_timer_get_time();
	return latest - oldStart;
  }
};

#endif // REDCROSS_MAIN_IPC_TIMER_HPP_
