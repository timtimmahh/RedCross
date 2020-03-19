/**
 * @file DeviceManager.hpp
 *
 * @brief DeviceManager class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_SENSOR_SRC_SENSORMANAGER_H
#define REDCROSS_LIB_SENSOR_SRC_SENSORMANAGER_H

#include "Perif.hpp"
#include "AccelGyro.h"
#include "GPS.h"
#include "LidarMap.h"
#include "USDist.h"
#include "BarPAT.h"
#include "SDFileIO.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <array>

#define SERVER_PORT 80

using namespace std;
using namespace perif;

template<class... Sensors>
class DeviceManager {
 private:
  // Use pointers to manually manage storage
  array<Perif*, sizeof...(Sensors)> devices;
  SDFileIO sdFile;
//  DataServer server;

  /**
   * Pass the sensor instances to the devices array and register server and
   * IO hooks.
   *
   * @param senses the created sensors
   */
  template<class Sensor, class... Others>
  explicit DeviceManager(Sensor* sensor, Others* ... senses)
      : devices{sensor, senses...}, sdFile() { init(); }

  void init() {
    if (sdFile.mount())
      sdFile.printCardInfo();
    else
      Serial.println("SD not ready!");
    hookSensors();
  }
 public:
  /**
   * Default constructor that creates sensor instances.
   */
  DeviceManager() : DeviceManager(new Sensors()...) {}

  /**
   * Clear sensors from memory. This should only be called when the device is
   * powering down or going to deep sleep.
   */
  ~DeviceManager() {
    for (auto item : devices)
      delete dynamic_cast<PerifBase*>(item);
  }

  /**
   * Registers hooks to the enabled sensors so when sensor data updates, it
   * can be saved to the SD card or sent to a remote server.
   * The remote server is the preferred location, but will fallback to the SD
   * card so the data can be sent when a connection is available.
   */
  void hookSensors() {
    for (Perif* item : devices) {
      item->begin();
      item->registerHook([this](DevMap& data) {
// if a path to the server is available, send it there instead
        if (sdFile.isReady()) {
          vector<string> v;
          v.reserve(data.size());
          const string& fName(data["name"] + ".csv");
          if (!sdFile.exists(fName)) {
            for (auto d : data)
              v.push_back(d.first);
            sdFile.writeFile(fName, v);
          }
          v.clear();
          for (auto d : data)
            v.push_back(d.second);
          if (sdFile.appendFile(fName, v)) Serial.print("Appended to: ");
          else Serial.print("Failed to append to: ");
          Serial.println(fName.c_str());
          string fileContent;
          sdFile.readFile(fName, fileContent);
          Serial.println(fileContent.c_str());
        }
        for (auto d : data) {
          Serial.print(d.first.c_str());
          Serial.print(": ");
          Serial.println(d.second.c_str());
        }
      });
    }
  }

  /**
   * Logs sensor information over serial.
   */
  void logSensors() {
    for (Perif* item : devices) {
      Serial.print("Perif: ");
      Serial.print(item->data["name"].c_str());
      Serial.print(" is connected? ");
      if (item->isConnected())
        Serial.println("yes");
      else
        Serial.println("no");
    }
  }

  void update() {
    for (Perif* item : devices)
      item->update();
  }

  /**
   * Getter for the DataServer to be accessible globally.
   *
   * @return the DataServer
   */
//  DataServer &getServer() const {
//	return const_cast<DataServer &>(server);
//  }
};

#endif // REDCROSS_LIB_SENSOR_SRC_SENSORMANAGER_H
