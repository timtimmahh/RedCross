/**
 * @dir main/include/
 *
 * @file DeviceManager.h
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

/**
 * The device manager for handling necessary sensor peripherals.
 *
 * @tparam Sensors the sensor types to manage
 */
template<class... Sensors>
class DeviceManager {
 private:
  /**
   * The sensors being managed.
   */
  array<Perif*, sizeof...(Sensors)> devices;
  /**
   * The SD card instance to enable offline storage.
   */
  SDFileIO sdFile;

  /**
   * Pass the sensor instances to the devices array and register server and
   * IO hooks.
   *
   * @tparam Sensor one sensor type
   * @tparam Others other sensor types
   * @param sensor one created sensor
   * @param senses the created sensors
   */
  template<class Sensor, class... Others>
  explicit DeviceManager(Sensor* sensor, Others* ... senses)
      : devices{sensor, senses...}, sdFile() { init(); }

  /**
   * Initialize the device manager and mount the SD card.
   */
  void init() {
    if (sdFile.mount())
	  sdFile.printCardInfo();
	else
	  LOG("SD not mounted");
    hookSensors();
  }
 public:
  /**
   * Default constructor that creates sensor instances.
   */
  DeviceManager()
	  : DeviceManager(new Sensors()...) {}

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
			  stringstream keys;
			  stringstream values;
			  for (auto i = data.rbegin(); i != data.rend(); ++i) {
				v.insert(v.begin(), i->second);
				keys << i->first << (*i == *data.begin() ? "\n" : ",");
				values << i->second << (*i == *data.begin() ? "\n" : ",");
			  }
			  sdFile.appendFile(fName, v);
			  LOG("\n", keys.str(), values.str());

			} else {
			  std::stringstream ss;
			  for (auto d : data) {
				ss << d.first << ": " << d.second;
			  }
			  LOG(ss.str());
			}
		  });
	}
  }

  /**
   * Logs sensor information over serial.
   */
  void logSensors() {
	std::stringstream ss;
	for (Perif *item : devices) {
	  ss << "Perif: " << item->data["name"] << " connected? ";
	  ss << (item->isConnected() ? "yes" : "no") << std::endl;
	}
	LOG(ss.str());
  }

  /**
   * Perform updates on all active devices.
   */
  void update() {
    for (Perif* item : devices)
      item->update();
  }
};

#endif // REDCROSS_LIB_SENSOR_SRC_SENSORMANAGER_H
