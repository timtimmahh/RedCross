//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_INCLUDE_DATASERVER_H
#define REDCROSS_INCLUDE_DATASERVER_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <SPIFFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_wifi.h>

using namespace perif;

/**
 * Creates a WebServer on ESP32 to display perif data.
 *
 * @tparam Sensors the perif types to handle
 */
class DataServer {

 protected:
  AsyncWebServer server;
  AsyncWebSocket ws;
  AsyncEventSource event;

 public:
  /**
   * Creates and sets up the WebServer with the specified port and sensors.
   *
   * @tparam SensorTypes the perif types to display
   * @param port the WebServer port number (default is 80 for browser use)
   * @param sensors the perif instances
   */
  explicit DataServer(int port)
	  : server(port), ws("/ws"), event("/event") {

	// add root page
	// Handles connections to the root of the web server.
	server.on("/index", [](AsyncWebServerRequest *req) {
	  req->send(SPIFFS, "/index.html", "text/html");
	});
  }

  void wifiConnected() {
	server.begin();
	Serial.println("HTTP server started.");
  }

  bool remoteAvailable() {
    return ws.enabled() && ws.count() > 0;
  }

/**
 * Wrapper so Arduino loop() function can handle client connections without
 * exposing the entire WebServer.
 */
  void handleClient() {

//	server.handleClient();
  }

/**
 * Variadic template termination function. Adds the not found page for bad
 * URLs.
 */
  void registerSensor() {
	server.onNotFound([](AsyncWebServerRequest *req) { req->send(400); });
	// Handles connections to an unknown path.
//		[&]() -> void {
//		  String message =
//			  String("File Not Found\n\n") + "URI: " + server.uri() +
//				  "\nMethod: " + ((server.method() == HTTP_GET) ? "GET" : "POST") +
//				  "\nArguments: " + server.args() + "\n";
//
//		  for (uint8_t i = 0; i < (uint8_t)server.args(); ++i)
//			message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
//
//		  server.send(404, "text/plain", message);
//		});
  }

/**
 * Adds all specified sensors to the WebServer.
 *
 * @tparam DataType the data type for a perif
 * @tparam SensorTypes the sensors types to register
 * @param sensor the first perif to register
 * @param sT the rest of the sensors to register
 */
  template<class S, class... SensorTypes>
  void registerSensor(S sensor, SensorTypes... sT) {
	Serial.print("Adding perif: ");
	Serial.println(sensor->name);
	server.on((String("/read") + sensor->name).c_str(),
			  [&](AsyncWebServerRequest *req) -> void {
//				sensor->as_json(sensor->getData());
//				String content;
//				serializeJson(data, content);
//				LOG_D("ESP - Data for " + sensor->name + ": " + content);
//				server.send(200, "text/plain", content);
			  });
	registerSensor(sT...);
  }
};

#endif //REDCROSS_INCLUDE_DATASERVER_H
