/**
 * @dir main/include/
 *
 * @file App.hpp
 *
 * @brief App class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_INCLUDE_APP_HPP_
#define REDCROSS_MAIN_INCLUDE_APP_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_pthread.h>
#include <chrono>

/**
 * The main app, only one instance of this can run.
 * Initializes WiFi, SD card, and non-volatile flash on startup.
 * It then sets up threads for sensors to be handled on interrupts or at a
 * timer interval.
 */
class App : public Thread {
 private:
  /**
   * Time to sleep in the main loop.
   */
  const nanoseconds sleep_time = nanoseconds{1000};
  /**
   * Whether non-volatile flash system is available.
   */
  bool nvsAvailable;
  /**
   * The WiFi instance.
   */
  WiFi wifi;
  /**
   * Device manager for handling sensor peripherals.
   */
  DeviceManager<PERIF_NAMES> *manager;
  /**
   * Initializes non-volatile storage.
   *
   * @return
   */
  static inline bool initNVS() {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES
		|| ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	  ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
	  ret = nvs_flash_init();
	}
	return ret == ESP_OK;
  }
 public:
  /**
   * Creates the application and initializes resources such as event loops.
   */
  App()
	  : Thread("app", CONFIG_MAIN_TASK_STACK_SIZE, 1, -1, true),
		nvsAvailable(initNVS()),
		wifi(),
		manager(nullptr) {
	esp_event_loop_create_default();
  }
  /**
   * Frees allocated resources on shutdown or sleep.
   */
  ~App() {
	delete manager;
	nvs_flash_erase();
	esp_event_loop_delete_default();
  }
 protected:
  /**
   * Initialize the app to run on the main thread.
   */
  void init() override {
	LOG("ESP32 Start");
	this_thread::sleep_for(sleep_time);
	// ESP32 pins need to be pulled down before connecting some peripherals
	manager = new DeviceManager<PERIF_NAMES>();
	// connect to the network or make the device available
	wifi.connect();
	LOG("Initialization complete");
  }
  /**
   * Perform any necessary updates. (Most are through interrupts)
   */
  void loop() override {
	// TODO change to asynchronous event
	manager->update();
	this_thread::sleep_for(sleep_time);
  }
};

#endif //REDCROSS_MAIN_INCLUDE_APP_HPP_
