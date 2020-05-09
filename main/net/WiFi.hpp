/**
 * @dir main/net/
 *
 * @file WiFi.hpp
 *
 * @brief WiFi class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_NET_WIFI_HPP_
#define REDCROSS_MAIN_NET_WIFI_HPP_

#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif.h>
#include "utils.h"

/**
 * The WiFi mode. Station or Access Point
 */
enum class WiFiMode {
  STA = 1, AP = 2
};

// WiFi AP credentials
// specify your WiFi SSID and password to connect to the Internet
// Web server port number (80 for default HTTP browser connections)
#define SSID        "You Can't Connect"
#define PASSWORD    "unlessIwantU2"

#define event(id, ...) \
case id: \
  LOG(#id); \
  CONCAT2(LINE, COUNT(__VA_ARGS__))(__VA_ARGS__) \
  break

/**
 * The WiFi manager.
 */
class WiFi {
 private:
  /**
   * The WiFi mode specified during creation.
   */
  WiFiMode mode;
  /**
   * Configures the WiFi connection for the specified mode.
   */
  void config_connection();
  /**
   * Configures the WiFi connection as an access point.
   */
  static void config_connection_ap();
  /**
   * Configures the WiFi connection as a station.
   */
  static void config_connection_sta();
 public:
  /**
   * Whether the device has a network connection.
   */
  bool connected = false;
  /**
   * Creates the WiFi instance with default mode Station.
   *
   * @param mode the WiFi mode
   */
  explicit WiFi(WiFiMode mode = WiFiMode::STA);
  /**
   * Free's any resources and turns off the WiFi card.
   */
  virtual ~WiFi();

  /**
   * Interrupt driven global WiFi event handler.
   *
   * @param arg this WiFi instance
   * @param event_base the ESP event base name
   * @param event_id the ESP event id
   * @param event_data any additional data for an event
   */
  static void wifi_event_handler(void *arg,
								 esp_event_base_t event_base,
								 int32_t event_id,
								 void *event_data);
  /**
   * Attempts to make a connection to any configured devices.
   */
  void connect();
};

/**
 * Processes events related to WiFi operations.
 *
 * @param wifi the WiFi manager instance.
 * @param event_id the WiFi event id
 * @param event_data any additional event data
 */
static inline void wifi_event(WiFi &wifi,
							  int32_t event_id,
							  void *event_data) {
  switch (event_id) {
	event(WIFI_EVENT_STA_START, esp_wifi_connect());
	event(WIFI_EVENT_STA_CONNECTED, wifi.connected = true);
	event(WIFI_EVENT_STA_DISCONNECTED,
	/* This is a workaround as ESP32 WiFi libs don't currently
	 * auto-reassociate. */
		  wifi.connected = false,
		  esp_wifi_connect());
	event(WIFI_EVENT_STA_AUTHMODE_CHANGE);
	event(WIFI_EVENT_STA_WPS_ER_SUCCESS);
	event(WIFI_EVENT_STA_WPS_ER_FAILED);
	event(WIFI_EVENT_STA_WPS_ER_TIMEOUT);
	event(WIFI_EVENT_STA_WPS_ER_PIN);
	event(WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP);
	event(WIFI_EVENT_AP_START);
	event(WIFI_EVENT_AP_STOP);
	event(WIFI_EVENT_AP_STACONNECTED);
	event(WIFI_EVENT_AP_STADISCONNECTED);
	event(WIFI_EVENT_AP_PROBEREQRECVED);
	default: break;
  }
}

/**
 * Processes events related to IP operations.
 *
 * @param wifi the WiFi manager instance.
 * @param event_id the IP event id
 * @param event_data any additional event data
 */
static inline void ip_event(WiFi &wifi,
							int32_t event_id,
							void *event_data) {
  switch (event_id) {
	event(IP_EVENT_STA_GOT_IP);
	event(IP_EVENT_STA_LOST_IP);
	event(IP_EVENT_AP_STAIPASSIGNED);
	event(IP_EVENT_GOT_IP6);
	event(IP_EVENT_ETH_GOT_IP);
	default: break;
  }
}
#endif // REDCROSS_MAIN_NET_WIFI_HPP_
