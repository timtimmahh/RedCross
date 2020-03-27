#include "redcross.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_pthread.h>
#include <chrono>
#include <thread>
#include <nvs.h>
#include <nvs_flash.h>

using namespace std::chrono;

const auto sleep_time = seconds{1};

class App : public Thread {
 private:
  WiFi wifi = WiFi();
 public:
  App() : Thread("main"), wifi() {
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES
		|| err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	  ESP_ERROR_CHECK(nvs_flash_erase());
	  err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
	esp_event_loop_create_default();
  }

  ~App() {
	esp_event_loop_delete_default();
	nvs_flash_deinit();
  }
 protected:
  void init() override {
	log("ESP32 Start");
	this_thread::sleep_for(sleep_time);
	manager = new DeviceManager<PERIF_NAMES>();
	wifi.connect();
	log("Initialization complete");
  }
  void loop() override {
	// TODO change to asynchronous event
	while (true) {
	  manager->update();
	  this_thread::sleep_for(sleep_time);
	}
  }
};

App app;

extern "C" void app_main() {
  app.start();
}
