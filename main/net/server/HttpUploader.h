/**
 * @dir main/net/server/
 *
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

/**
 * A basic data structure representing a network response.
 */
struct response {
  /**
   * ESP error value.
   */
  esp_err_t err;
  /**
   * HTTP response code.
   */
  int statusCode;
  /**
   * HTTP content length.
   */
  int contentLength;
};

/**
 * Performs uploading to a remote receiver. This is thread-safe and can
 * send data packets whenever data is available, or it can send sequences as
 * a batch request.
 */
class HttpUploader {
 private:
  /**
   * The ESP HTTP client configuration.
   */
  esp_http_client_config_t config;
  /**
   * The data to upload.
   */
  Queue<string> &data;
  /**
   * An event handler to use for HTTP callbacks.
   *
   * @return ESP error
   */
  esp_err_t (*_http_event_handler)(esp_http_client_event_t *) =
  [](esp_http_client_event_t *evt) -> esp_err_t {
	return ESP_OK;
  };
  /**
   * Uploads all data that is currently in the data queue.
   *
   * @return the results of the batch request
   */
  list<response> batchRequest();
  /**
   * Performs the actual HTTP request.
   *
   * @param client the ESP HTTP client
   * @return the HTTP response
   */
  response perform(esp_http_client_handle_t &client);
 public:
  /**
   * Creates an HTTP uploader with a thread-safe data queue.
   *
   * @param data the data queue to use
   */
  explicit HttpUploader(Queue<string> &data);
  /**
   * Uploads a all data from the queue.
   *
   * @return the HTTP responses
   */
  list<response> upload();
};

#endif // REDCROSS_LIB_NET_SERVER_HTTPUPLOADER_H_
