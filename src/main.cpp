#include "redcross.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_pthread.h>


void *arduinoTask(void *pvParameter) {
  Serial.begin(115200);
  Serial.println("Starting...");
  Serial.println("ESP32 Setup");
  manager = new DeviceManager<PERIF_NAMES>();
//  ESP_LOGD(__FILE__, "Starting...");
//  ESP_LOGD(__FILE__, "ESP32 Setup");
//  setupWiFi();
  // notify web server, we have internet connection
  // TODO change to asynchronous event
//  manager.getServer().wifiConnected();
  delay(1000);
  Serial.println("Logging...");
  manager->logSensors();
  delay(1000);
  Serial.println("Starting loop...");
  unsigned int count = 1;
  while (true) {
	Serial.print("loop");
	Serial.println(count++);
	manager->update();
	delay(1000);
  }
}

extern "C" void app_main(void) {
  initArduino();
  pthread_t arduino;
  esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
  cfg.stack_size = 1024 * 4;
  cfg.inherit_cfg = true;
  esp_pthread_set_cfg(&cfg);

  pthread_create(&arduino, nullptr, arduinoTask, nullptr);
}
