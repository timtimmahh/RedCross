/**
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

enum class WiFiMode {
  STA = 1, AP = 2
};

// WiFi AP credentials
// specify your WiFi SSID and password to connect to the Internet
// Web server port number (80 for default HTTP browser connections)
#define SSID        "Timothys-Fedora"
#define PASSWORD    "UHDUNNO12"

class WiFi {
 private:
  WiFiMode mode;
  bool connected = false;
  void config_connection();
  static void config_connection_ap();
  static void config_connection_sta();
 public:
  WiFi();
  virtual ~WiFi();

  static void wifi_event_handler(void *arg,
								 esp_event_base_t event_base,
								 int32_t event_id,
								 void *event_data);
  void connect();
};

static inline void wifi_event(WiFi &wifi,
							  int32_t event_id,
							  void *event_data) {
  switch (event_id) {
	case WIFI_EVENT_STA_START: esp_wifi_connect();
	  break;
	case WIFI_EVENT_STA_CONNECTED:

	  break;
	case WIFI_EVENT_STA_DISCONNECTED:
	  /* This is a workaround as ESP32 WiFi libs don't currently
		 auto-reassociate. */
	  esp_wifi_connect();
	  break;
	case WIFI_EVENT_STA_AUTHMODE_CHANGE:break;
	case WIFI_EVENT_STA_WPS_ER_SUCCESS:
	case WIFI_EVENT_STA_WPS_ER_FAILED:
	case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
	case WIFI_EVENT_STA_WPS_ER_PIN:
	case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP: break;
	case WIFI_EVENT_AP_START:break;
	case WIFI_EVENT_AP_STOP:break;
	case WIFI_EVENT_AP_STACONNECTED:break;
	case WIFI_EVENT_AP_STADISCONNECTED:break;
	case WIFI_EVENT_AP_PROBEREQRECVED:break;
	default: break;
  }
}

static inline void ip_event(WiFi &wifi,
							int32_t event_id,
							void *event_data) {
  switch (event_id) {
	case IP_EVENT_STA_GOT_IP:
	case IP_EVENT_STA_LOST_IP:break;
	case IP_EVENT_AP_STAIPASSIGNED:break;
	case IP_EVENT_GOT_IP6:break;
	case IP_EVENT_ETH_GOT_IP: break;
	default: break;
  }
}
#endif // REDCROSS_MAIN_NET_WIFI_HPP_
