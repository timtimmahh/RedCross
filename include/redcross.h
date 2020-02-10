//
// Created by tim on 2/9/20.
//

#ifndef REDCROSS_INCLUDE_REDCROSS_H
#define REDCROSS_INCLUDE_REDCROSS_H

#include "Arduino.h"

// DEBUG flag set to enable logging (defining before including Arduino.h fails
// to compile. See https://github.com/esp8266/Arduino/issues/3806
#define DEBUG
#include "DebugUtils.h"
// ESP32 global configurations
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

#define BAUD        115200

// WiFi AP credentials
// specify your WiFi SSID and password to connect to the Internet
#define SSID        "................."
#define PASSWORD    "................."
// Web server port number (80 for default HTTP browser connections)
#define SERVER_PORT 80

/**
 * Sets up WiFi with the default SSID and password.
 */
void setupWiFi() {

  Serial.println(String("Connecting to '") + SSID + "'");

  // establish connection to WiFi access point
  WiFiClass::mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  // wait until connection is established
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

#elif ARDUINO_AVR_UNO
#define BAUD 9600
#endif

#endif //REDCROSS_INCLUDE_REDCROSS_H
