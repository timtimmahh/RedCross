/**
 * @dir main/ipc/timer/
 *
 * @file Timer.hpp
 *
 * @brief Timer class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_IPC_TIMER_HPP_
#define REDCROSS_MAIN_IPC_TIMER_HPP_

#include <esp_timer.h>

/**
 * A timer for asynchronous operations and thread communication.
 */
class Timer {
 private:
  /**
   * The most recent time.
   */
  uint64_t latest;
  /**
   * The ESP timer handle.
   */
  esp_timer_handle_t timer;
  /**
   * Timer callback function for interrupt driven timer implementations.
   *
   * @param args any callback arguments
   */
  static void timer_cb(void *args) {
    // TODO
  }

 public:
  /**
   * Creates and initializes a timer instance.
   */
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
  /**
   * Start the timer (if not already).
   */
  virtual void start() {

  }

  /**
   * Starts tracking elapsed time.
   */
  void startTimeLapse() {
	latest = esp_timer_get_time();
  }

  /**
   * Returns the most recent update time without refreshing.
   *
   * @return the most recent time
   */
  uint64_t getLatestUpdate() const {
	return latest;
  }

  /**
   * Returns the total time since the system started.
   *
   * @return the total system runtime
   */
  uint64_t getRuntime() {
	return esp_timer_get_time();
  }

  /**
   * Obtains the elapsed time since startTimeLapse() was called.
   *
   * @return the elapsed time.
   */
  uint64_t elapsedTime() {
	uint64_t oldStart = latest;
	latest = esp_timer_get_time();
	return latest - oldStart;
  }
};

#endif // REDCROSS_MAIN_IPC_TIMER_HPP_
