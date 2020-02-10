//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_INCLUDE_SENSORSERVER_H
#define REDCROSS_INCLUDE_SENSORSERVER_H

#include "redcross.h"
#include "Sensor.h"
#include "index.h"

/**
 * Creates a WebServer on ESP32 to display sensor data.
 *
 * @tparam Ts the sensor types to handle
 */
template<typename... Ts>
class SensorServer {
private:
  WebServer server;
  // the sensor instances
  std::tuple<Ts...> sensors;

  /**
   * Handles connections to the root of the web server.
   */
  std::function<void()> handleRoot = [&] {
    server.send(200, "text/html", MAIN_page[0]);
  };

  /**
   * Handles connections to an unknown path.
   */
  std::function<void()> handleNotFound = [&]() -> void {
    String message =
        String("File Not Found\n\n") + "URI: " + server.uri() +
            "\nMethod: " + ((server.method() == HTTP_GET) ? "GET" : "POST") +
            "\nArguments: " + server.args() + "\n";

    for (uint8_t i = 0; i < (uint8_t)server.args(); ++i)
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";

    server.send(404, "text/plain", message);
  };

public:
  /**
   * Creates and sets up the WebServer with the specified port and sensors.
   *
   * @tparam SensorTypes the sensor types to display
   * @param port the WebServer port number (default is 80 for browser use)
   * @param sensors the sensor instances
   */
  template<typename... SensorTypes>
  explicit SensorServer(int port, SensorTypes&... sensors)
  : server(port)
  , sensors(sensors...)
  {
    LOG("Main HTML Page: ")
    LOG(MAIN_page[0])
    // add root page
    server.on("/", handleRoot);
    // register all sensors with the WebServer
    registerSensor(sensors...);
    server.begin();
    LOG_D("HTTP server started.");
  }

  /**
   * Wrapper so Arduino loop() function can handle client connections without
   * exposing the entire WebServer.
   */
  void handleClient() {
    server.handleClient();
  }

  /**
   * Variadic template termination function. Adds the not found page for bad
   * URLs.
   */
  void registerSensor() {
    server.onNotFound(handleNotFound);
  }

  /**
   * Adds all specified sensors to the WebServer.
   *
   * @tparam DataType the data type for a sensor
   * @tparam SensorTypes the sensors types to register
   * @param sensor the first sensor to register
   * @param sT the rest of the sensors to register
   */
  template<typename DataType, class... SensorTypes>
  void registerSensor(Sensor<DataType>& sensor, SensorTypes&... sT) {
    LOG_D("Adding sensor: " + sensor.name());
    server.on(String("/read") + sensor.name(), [&] {
      String data = sensor.dataAsString(sensor.getSensorData());
      LOG_D("ESP - Data for " + sensor.name() + ": " + data);
      server.send(200, "text/plain", data);
    });
    registerSensor(sT...);
  }

};

#endif //REDCROSS_INCLUDE_SENSORSERVER_H
