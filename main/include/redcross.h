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
#include "ErrorLog.hpp"
#include <nvs_flash.h>
#include "utils.h"

#include "timer/Timer.hpp"
#include "Event.hpp"
#include "Queue.hpp"
#include "Thread.hpp"
#include "DeviceManager.h"
#include "WiFi.hpp"

#define MAP(Perif) perif::Perif

#define PERIF_NAMES MAP(LidarMap)

#endif //REDCROSS_INCLUDE_REDCROSS_H