//
// Created by tim on 2/25/20.
//

#ifndef REDCROSS_SRC_NET_NETCFG_H
#define REDCROSS_SRC_NET_NETCFG_H

#include <cstring>
#include <WiFi.h>
#include <ESPmDNS.h>


// WiFi AP credentials
// specify your WiFi SSID and password to connect to the Internet
// Web server port number (80 for default HTTP browser connections)
#define SSID        "Timothys-Fedora"
#define PASSWORD    "UHDUNNO12"

/* FreeRTOS event group to signal when we are connected*/
/*static EventGroupHandle_t s_wifi_event_group;

*//* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? *//*
const int WIFI_CONNECTED_BIT = BIT0;

static int s_retry_num = 0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch(event->event_id) {
  case SYSTEM_EVENT_STA_START:
	esp_wifi_connect();
	break;
  case SYSTEM_EVENT_STA_GOT_IP:
	ESP_LOGI(__FILE__, "got ip:%s",
			 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
	s_retry_num = 0;
	xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
  {
	if (s_retry_num < 10) {
	  esp_wifi_connect();
	  xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	  s_retry_num++;
	  ESP_LOGI(__FILE__,"retry to connect to the AP");
	}
	ESP_LOGI(__FILE__,"connect to the AP fail\n");
	break;
  }
  default:
	break;
  }
  return ESP_OK;
}

void wifi_init_sta()
{
  s_wifi_event_group = xEventGroupCreate();

  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, nullptr))

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
  ESP_ERROR_CHECK(esp_wifi_init(&cfg))
  wifi_config_t wifi_config = {
	  .sta = {
		  .ssid = SSID,
		  .password = PASSWORD
	  },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) )
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) )
  ESP_ERROR_CHECK(esp_wifi_start() )

  ESP_LOGI(__FILE__, "wifi_init_sta finished.");
  ESP_LOGI(__FILE__, "connect to ap SSID:%s password:%s",
		   SSID, PASSWORD);
}*/

/*void setupWiFi() {

  ESP_LOGD(__FILE__, "Starting WiFi configuration.");
//Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	ESP_ERROR_CHECK(nvs_flash_erase())
	ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret)

  ESP_LOGI(__FILE__, "ESP_WIFI_MODE_STA");
  wifi_init_sta();
}*/



/**
 * Sets up WiFi with the default SSID and password.
 */
void setupWiFi() {

  // establish connection to WiFi access point
  WiFiClass::mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  // wait until connection is established
  // TODO change to asynchronous event
  while (WiFiClass::status() != WL_CONNECTED) {
	delay(500);
	Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi Connected.");
  Serial.print("Server location: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin("esp32"))
	Serial.println("MDNS responder started");
}

#endif //REDCROSS_SRC_NET_NETCFG_H
