//
// Created by tim on 2/2/20.
//

#ifndef REDCROSS_INCLUDE_DEBUGUTILS_H
#define REDCROSS_INCLUDE_DEBUGUTILS_H

#include "RealTime.h"

// define logging macros for debugging, if not building for release
#ifdef DEBUG

#define LOG(str) Serial.println(str);
#define LOG_D(str)  \
      Serial.println(RTC::timeString() + ": " \
                    + __PRETTY_FUNCTION__ + ' ' \
                    + __FILE__ + ':' \
                    + __LINE__ + ' ' + str)
#else
// debugging is disabled, define logging macros to nothing to allow compiling
// without modifying code.
#define LOG(str)
#define LOG_D(str)
#endif

#endif //REDCROSS_INCLUDE_DEBUGUTILS_H
