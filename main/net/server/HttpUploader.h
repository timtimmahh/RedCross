/**
 * @file HttpUploader.h
 *
 * @brief HttpUploader class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_NET_SERVER_HTTPUPLOADER_H_
#define REDCROSS_LIB_NET_SERVER_HTTPUPLOADER_H_

#include <functional>
#include <list>
#include "Queue.hpp"
#include <esp_system.h>
#include <esp_http_client.h>

#define SERVER_URL "http://something..."

using namespace std;

struct response {
  esp_err_t err;
  int statusCode;
  int contentLength;
};

class HttpUploader {
 private:
  esp_http_client_config_t config;
  Queue<string> &data;
  esp_err_t (*_http_event_handler)(esp_http_client_event_t *) =
  [](esp_http_client_event_t *evt) -> esp_err_t {
	return ESP_OK;
  };
  list<response> batchRequest();
  response perform(esp_http_client_handle_t &client);
 public:
  explicit HttpUploader(Queue<string> &data);
  list<response> upload();
};

#endif // REDCROSS_LIB_NET_SERVER_HTTPUPLOADER_H_
