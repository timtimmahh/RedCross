/**
 * @file redcross.h
 *
 * @brief RedCross global includes.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_INCLUDE_REDCROSS_H
#define REDCROSS_INCLUDE_REDCROSS_H

// DEBUG flag set to enable logging (defining before including Arduino.h fails
// to compile. See https://github.com/esp8266/Arduino/issues/3806
// uncomment for debug output
//#define DEBUG
#include <esp_err.h>
#include "DeviceManager.h"
#include "DataServer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Arduino.h"
#include "netcfg.h"
#include <esp_log.h>
#include <nvs_flash.h>


#define MAP(Perif) perif::Perif
#define NAME(perif) #perif
#define NAMED NAME(LidarMap)
#define ACTIVATED LidarMap
//, NAME(USDist), NAME(GPS)
//, USDist, GPS
//, MAP(USDist), MAP(GPS)

#define PERIF_NAMES MAP(GPS)


DeviceManager<PERIF_NAMES>* manager;

/*void scanI2C() {
  byte error, address; //variable for error and I2C address
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  delay(5000); // wait 5 seconds for the next I2C scan
}*/

#endif //REDCROSS_INCLUDE_REDCROSS_H