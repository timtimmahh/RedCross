/**
 * @file AwsThingShadow.hpp
 *
 * @brief AWSThingShadow class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_NET_IOT_AWSTHINGSHADOW_HPP_
#define REDCROSS_LIB_NET_IOT_AWSTHINGSHADOW_HPP_

//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>
//#include <aws_iot_config.h>
//#include <aws_iot_log.h>
//#include <aws_iot_version.h>
//#include <aws_iot_mqtt_client_interface.h>
//#include <aws_iot_shadow_interface.h>

static const char *TAG = "shadow";

#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200

/* CA Root certificate, device ("Thing") certificate and device
 * ("Thing") key.
   Example can be configured one of two ways:
   "Embedded Certs" are loaded from files in "certs/" and embedded into the app binary.
   "Filesystem Certs" are loaded from the filesystem (SD card, etc.)
   See example README for more details.
*/
/*
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t certificate_pem_crt_start[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t certificate_pem_crt_end[] asm("_binary_certificate_pem_crt_end");
extern const uint8_t private_pem_key_start[] asm("_binary_private_pem_key_start");
extern const uint8_t private_pem_key_end[] asm("_binary_private_pem_key_end");

void aws_iot_task(void *param) {
  IoT_Error_t rc = FAILURE;

  char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
  size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);
  float temperature = 0.0;

  bool windowOpen = false;
  jsonStruct_t windowActuator;
  windowActuator.cb = NULL;
  windowActuator.pData = &windowOpen;
  windowActuator.pKey = "windowOpen";
  windowActuator.type = SHADOW_JSON_BOOL;
  windowActuator.dataLength = sizeof(bool);

  jsonStruct_t temperatureHandler;
  temperatureHandler.cb = NULL;
  temperatureHandler.pKey = "temperature";
  temperatureHandler.pData = &temperature;
  temperatureHandler.type = SHADOW_JSON_FLOAT;
  temperatureHandler.dataLength = sizeof(float);

  ESP_LOGI(TAG, "AWS IoT SDK Version %d.%d.%d-%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

  // initialize the mqtt client
  AWS_IoT_Client mqttClient;

  ShadowInitParameters_t sp = ShadowInitParametersDefault;
  sp.pHost = AWS_IOT_MQTT_HOST;
  sp.port = AWS_IOT_MQTT_PORT;

  sp.pClientCRT = (const char *)certificate_pem_crt_start;
  sp.pClientKey = (const char *)private_pem_key_start;
  sp.pRootCA = (const char *)aws_root_ca_pem_start;
  sp.enableAutoReconnect = false;
  sp.disconnectHandler = NULL;

  *//* Wait for WiFI to show as connected *//*
//  xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
//                      false, true, portMAX_DELAY);

  ESP_LOGI(TAG, "Shadow Init");
  rc = aws_iot_shadow_init(&mqttClient, &sp);
  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "aws_iot_shadow_init returned error %d, aborting...", rc);
    abort();
  }

  ShadowConnectParameters_t scp = ShadowConnectParametersDefault;
  scp.pMyThingName = "FloodDetector";
//  scp.pMqttClientId = CONFIG_AWS_EXAMPLE_CLIENT_ID;
//  scp.mqttClientIdLen = (uint16_t) strlen(CONFIG_AWS_EXAMPLE_CLIENT_ID);

  ESP_LOGI(TAG, "Shadow Connect");
  rc = aws_iot_shadow_connect(&mqttClient, &scp);
  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "aws_iot_shadow_connect returned error %d, aborting...", rc);
    abort();
  }

  *//*
   * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
   *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
   *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
   *//*
  rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "Unable to set Auto Reconnect to true - %d, aborting...", rc);
    abort();
  }

  rc = aws_iot_shadow_register_delta(&mqttClient, &windowActuator);

  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "Shadow Register Delta Error");
  }
//  temperature = STARTING_ROOMTEMPERATURE;

  // loop and publish a change in temperature
  while(NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) {
    rc = aws_iot_shadow_yield(&mqttClient, 200);
    if(NETWORK_ATTEMPTING_RECONNECT == rc *//*|| shadowUpdateInProgress*//*) {
      rc = aws_iot_shadow_yield(&mqttClient, 1000);
      // If the client is attempting to reconnect, or already waiting on a shadow update,
      // we will skip the rest of the loop.
      continue;
    }
    ESP_LOGI(TAG, "=======================================================================================");
    ESP_LOGI(TAG, "On Device: window state %s", windowOpen ? "true" : "false");
//    simulateRoomTemperature(&temperature);

    rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
    if(SUCCESS == rc) {
      rc = aws_iot_shadow_add_reported(JsonDocumentBuffer, sizeOfJsonDocumentBuffer, 2, &temperatureHandler,
                                       &windowActuator);
      if(SUCCESS == rc) {
        rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
        if(SUCCESS == rc) {
          ESP_LOGI(TAG, "Update Shadow: %s", JsonDocumentBuffer);
//          rc = aws_iot_shadow_update(&mqttClient, CONFIG_AWS_EXAMPLE_THING_NAME, JsonDocumentBuffer,
//                                     ShadowUpdateStatusCallback, NULL, 4, true);
//          shadowUpdateInProgress = true;
        }
      }
    }
    ESP_LOGI(TAG, "*****************************************************************************************");
    ESP_LOGI(TAG, "Stack remaining for task '%s' is %d bytes", pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL));

    vTaskDelay(1000 / portTICK_RATE_MS);
  }

  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "An error occurred in the loop %d", rc);
  }

  ESP_LOGI(TAG, "Disconnecting");
  rc = aws_iot_shadow_disconnect(&mqttClient);

  if(SUCCESS != rc) {
    ESP_LOGE(TAG, "Disconnect error %d", rc);
  }

  vTaskDelete(NULL);
}*/
class AWSThingShadow {

};

#endif // REDCROSS_LIB_NET_IOT_AWSTHINGSHADOW_HPP_
