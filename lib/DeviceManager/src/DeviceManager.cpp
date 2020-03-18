/**
 * @file DeviceManager.cpp
 *
 * @brief DeviceManager class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "DeviceManager.h"

template<class... Sensors>
template<class Sensor, class... Others>
DeviceManager<Sensors...>::DeviceManager(Sensor* sensor,
                                         Others* ... senses)
    : devices{sensor, senses...} { init(); }

template<class... Sensors>
DeviceManager<Sensors...>::DeviceManager()
    : devices{new Sensors()...} { init(); }

template<class... Sensors>
void DeviceManager<Sensors...>::init() {
  SDFileIO& sdFile = SDFileIO::instance();
  if (sdFile.mount())
    sdFile.printCardInfo();
  else
    Serial.println("SD not ready!");
  hookSensors();
}

template<class... Sensors>
DeviceManager<Sensors...>::~DeviceManager() {
  for (auto item : devices)
    delete dynamic_cast<PerifBase*>(item);
}

template<class... Sensors>
void DeviceManager<Sensors...>::devHook(DevMap& data) {
  SDFileIO& sdFile = SDFileIO::instance();
  // if a path to the server is available, send it there instead
  if (sdFile.isReady()) {
    vector<const char*> v;
    const char* fName = (data["name"] + ".csv").c_str();
    if (!sdFile.exists(fName)) {
      for (auto d : data)
        v.push_back(d.first);
      sdFile.writeFile(fName, v);
    }
    v.clear();
    for (auto d : data)
      v.push_back(d.second.c_str());
    sdFile.appendFile(fName, v);
    string fileContent;
    sdFile.readFile(fName, fileContent);
    Serial.println(fileContent.c_str());
  } else {
    for (auto d : data) {
      Serial.print(d.first);
      Serial.print(": ");
      Serial.println(d.second.c_str());
    }
  }
}

template<class... Sensors>
void DeviceManager<Sensors...>::hookSensors() {
  for (Perif* item : devices) {
    item->begin();
    item->registerHook(this->devHook);
  }
}

template<class... Sensors>
void DeviceManager<Sensors...>::logSensors() {
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

template<class... Sensors>
void DeviceManager<Sensors...>::update() {
  for (Perif* item : devices)
    item->update();
}

//template<class... Sensors>
//DataServer &getServer() {
//	return const_cast<DataServer &>(server);
//  }
