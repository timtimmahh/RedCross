#include "redcross.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_pthread.h>
#include <chrono>
#include <thread>

using namespace std::chrono;

class App : public Thread {
 private:
  const nanoseconds sleep_time = nanoseconds{1000};
  bool nvsAvailable;
  WiFi wifi;
  DeviceManager<PERIF_NAMES> *manager;
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
  App()
	  : Thread("app", CONFIG_MAIN_TASK_STACK_SIZE, 1, -1, true),
		nvsAvailable(initNVS()),
		wifi(),
		manager(nullptr) {
	esp_event_loop_create_default();
  }

  ~App() {
	delete manager;
	nvs_flash_erase();
	esp_event_loop_delete_default();
  }
 protected:
  void init() override {
	LOG("ESP32 Start");
	this_thread::sleep_for(sleep_time);
	manager = new DeviceManager<PERIF_NAMES>();
	wifi.connect();
	LOG("Initialization complete");
  }
  void loop() override {
	// TODO change to asynchronous event
	manager->update();
	this_thread::sleep_for(sleep_time);
  }
};

extern "C" void app_main() {
  try {
	App().start();
  } catch (const runtime_error &e) {
	LOG("Exception caught: ", e.what());
  }
}
