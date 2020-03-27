/**
 * @file HttpUploader.cpp
 *
 * @brief HttpUploader class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "HttpUploader.h"

HttpUploader::HttpUploader(Queue<string> &data)
	: data(data) {
  config.url = SERVER_URL;
  config.method = HTTP_METHOD_POST;
  config.timeout_ms = 5000;
  config.is_async = false;
//  config.event_handler = _http_event_handler;
}

list<response> HttpUploader::batchRequest() {
  esp_http_client_handle_t client = esp_http_client_init(&config);
  list<response> responses;
  uint8_t retryCount = 0;
  while (!data.empty()) {
	if (data.pop_if(
		[this, &retryCount, &responses, &client](const string &front) {
		  if (retryCount == 0)
			esp_http_client_set_post_field(client, front.c_str(), front.size());
		  auto resp = perform(client);
		  if (resp.err == ESP_OK)
			responses.push_back(resp);
		  else return false;
		  return true;
		}))
	  retryCount = 0;
	else if (retryCount == 3) break;
	else retryCount++;
  }
  esp_http_client_cleanup(client);
  return responses;
}

response HttpUploader::perform(esp_http_client_handle_t &client) {
  esp_err_t err = esp_http_client_perform(client);
  if (err == ESP_OK)
	return {
		.err = err,
		.statusCode = esp_http_client_get_status_code(client),
		.contentLength = esp_http_client_get_content_length(client)
	};
  else return {.err = err, .statusCode = -1, .contentLength = -1};
}

list<response> HttpUploader::upload() {
  return batchRequest();
}
