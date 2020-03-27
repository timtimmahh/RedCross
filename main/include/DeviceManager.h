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
#include "DataServer.h"
#include "SDFileIO.hpp"
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
	  log("SD not mounted");
    hookSensors();
  }
 public:
  /**
   * Default constructor that creates sensor instances.
   */
  DeviceManager()
	  : devices{new Sensors()...},
		sdFile() { init(); }

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
	  // TODO - change to event loop
	  //      - add network database/aws iot
	  item->registerHook(
		  [this](DevMap &data) {
			if (sdFile.isReady()) {
			  vector<string> v;
			  v.reserve(data.size());
			  const string &fName(data["name"] + ".csv");
			  if (!sdFile.exists(fName)) {
				for (auto d : data)
				  v.push_back(d.first);
				sdFile.writeFile(fName, v);
			  }
			  v.clear();
			  for (auto d : data)
				v.push_back(d.second);
			  sdFile.appendFile(fName, v);

			  string fileContent;
			  sdFile.readFile(fName, fileContent);
			  log(fileContent);
			} else {
			  std::stringstream ss;
			  for (auto d : data) {
				ss << d.first << ": " << d.second;
			  }
			  log(ss.str());
			}
		  });
	}
  }

  /**
   * Logs sensor information over serial.
   */
  void logSensors() {
	std::stringstream ss;
    for (Perif* item : devices) {
	  ss << "Perif: " << item->data["name"] << " connected? ";
	  ss << (item->isConnected() ? "yes" : "no") << std::endl;
	}
	log(ss.str());
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
