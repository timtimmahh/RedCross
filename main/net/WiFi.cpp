/**
 * @file WiFi.cpp
 *
 * @brief WiFi class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include <cstring>
#include "WiFi.hpp"
#include "utils.h"

void WiFi::wifi_event_handler(void *arg,
							  esp_event_base_t event_base,
							  int32_t event_id,
							  void *event_data) {
  auto *wifi = reinterpret_cast<WiFi *>(arg);
  if (event_base == WIFI_EVENT)
	wifi_event(*wifi, event_id, event_data);
  else if (event_base == IP_EVENT)
	ip_event(*wifi, event_id, event_data);
}
WiFi::WiFi()
	: mode(WiFiMode::STA), connected(false) {
  tcpip_adapter_init();
  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
							 WiFi::wifi_event_handler, this);
  esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID,
							 WiFi::wifi_event_handler, this);
}

WiFi::~WiFi() {
  esp_event_handler_unregister(IP_EVENT, ESP_EVENT_ANY_ID,
							   &WiFi::wifi_event_handler);
  esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
							   &WiFi::wifi_event_handler);
  esp_wifi_disconnect();
  esp_wifi_stop();
  esp_wifi_deinit();
}
void WiFi::config_connection() {
  if (mode == WiFiMode::AP)
	config_connection_ap();
  else if (mode == WiFiMode::STA)
	config_connection_sta();
}

void WiFi::config_connection_ap() {
  wifi_config_t wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  copy_min_to_buffer(SSID, wifi_config.ap.ssid);
  copy_min_to_buffer(PASSWORD, wifi_config.ap.password);
  wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
  wifi_config.ap.max_connection = 4;
  log("Setting WiFi configuration SSID ", SSID, "...");
  esp_wifi_set_mode(WIFI_MODE_AP);
  esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
}

void WiFi::config_connection_sta() {
  wifi_config_t wifi_config;

  memset(&wifi_config, 0, sizeof(wifi_config));
  copy_min_to_buffer(SSID, wifi_config.sta.ssid);
  copy_min_to_buffer(PASSWORD, wifi_config.sta.password);
  wifi_config.sta.scan_method = WIFI_FAST_SCAN;
  wifi_config.sta.bssid_set = false;
  wifi_config.sta.channel = 0;
  wifi_config.sta.listen_interval = 0;
  log("Setting WiFi configuration SSID ", SSID, "...");
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

void WiFi::connect() {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  config_connection();
  esp_wifi_start();
}