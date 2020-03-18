/**
 * @file DeviceManager.hpp
 *
 * @brief DeviceManager class declaration.
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
#include "DataServer.h"
#include <cstdio>
#include <cstdlib>
#include <map>
#include <array>

#define SERVER_PORT 80

using namespace std;

template<class... Sensors>
class DeviceManager {
 private:
  // Use pointers to manually manage storage
  array<perif::Perif*, sizeof...(Sensors)> devices;
//  DataServer server;

  /**
   * Pass the sensor instances to the devices array and register server and
   * IO hooks.
   *
   * @param senses the created sensors
   */
  template<class Sensor, class... Others>
  explicit DeviceManager(Sensor* sensor, Others*... senses);
  void init();
 public:
  /**
   * Default constructor that creates sensor instances.
   */
  DeviceManager();

  /**
   * Clear sensors from memory. This should only be called when the device is
   * powering down or going to deep sleep.
   */
  ~DeviceManager();

  void devHook(DevMap& data);

  /**
   * Registers hooks to the enabled sensors so when sensor data updates, it
   * can be saved to the SD card or sent to a remote server.
   * The remote server is the preferred location, but will fallback to the SD
   * card so the data can be sent when a connection is available.
   */
  void hookSensors();

  /**
   * Logs sensor information over serial.
   */
  void logSensors();

  void update();

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
