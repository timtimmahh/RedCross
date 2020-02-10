//
// Created by tim on 2/9/20.
//

#ifndef REDCROSS_INCLUDE_REALTIME_H
#define REDCROSS_INCLUDE_REALTIME_H

#include <Arduino.h>
#include <ErriezDS1302.h>
#include <WiFi.h>
#include <ctime>


// Network Time Protocol definitions
#define NTP_SERVER            "pool.ntp.org" // global NTP server
#define GMT_OFFSET_SEC        -18000 // US Eastern time offset from GMT
#define DAYLIGHT_OFFSET_SEC   3600 // daylight savings offset

static bool ntpTimeConfigured = false;

/**
 * Obtains the time from the NTP server.
 *
 * @return the current time
 */
static struct tm getNTPTime() {
  if (!ntpTimeConfigured) {
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    ntpTimeConfigured = true;
  }
  struct tm timeInfo{};
  if (!getLocalTime(&timeInfo))
    Serial.println("Failed to get NTP time.");
  return timeInfo;
}

// specify pins for DS1302 RTC
#ifdef ARDUINO_ARCH_ESP32
#define CLK_PIN   0
#define IO_PIN    4
#define CE_PIN    5
#elif ARDUINO_AVR_UNO
#define CLK_PIN   2
#define IO_PIN    3
#define CE_PIN    4
#endif

/**
 * Wrapper for RTC management. Handles setting RTC time from NTP server.
 */
class RTC {
private:
  static RTC *globalRTC;
  static bool running;
  DS1302 rtc;
  /**
   * Constructor for RTC.
   */
  RTC() : rtc(CLK_PIN, IO_PIN, CE_PIN) {
    running = rtc.begin();
    // can only get NTP time if Internet is available.
    if (WiFi.isConnected()) {
      struct tm timeInfo = getNTPTime();
      rtc.setDateTime(convNTPTime(timeInfo));
    }
  }
  /**
   * Converts the NTP time struct to DS1302 date time.
   *
   * @param timeInfo the time from the NTP server
   * @return DS1302_DateTime address
   */
  static DS1302_DateTime* convNTPTime(struct tm timeInfo)  {
    return new DS1302_DateTime{
        .second = (uint8_t)timeInfo.tm_sec,
        .minute = (uint8_t)timeInfo.tm_min,
        .hour = (uint8_t)timeInfo.tm_hour,
        .dayWeek = (uint8_t)timeInfo.tm_wday,
        .dayMonth = (uint8_t)timeInfo.tm_mday,
        .month = (uint8_t)timeInfo.tm_mon,
        .year = (uint8_t)timeInfo.tm_year
    };
  }
public:
  /**
   * Obtains the RTC singleton.
   *
   * @return the RTC singleton
   */
  static RTC& instance() {
    if (globalRTC == nullptr || !running) {
      delete globalRTC;
      globalRTC = new RTC();
    }
    return *globalRTC;
  }
  /**
   * Converts the DS1302_DateTime to a human readable String.
   *
   * @return formatted time String
   */
  static String timeString() {
    if (WiFi.isConnected())
      return dtToString(*convNTPTime(getNTPTime()));
    DS1302_DateTime dt;
    RTC& inst = instance();
    if (!inst.rtc.getDateTime(&dt))
      Serial.println("Failed to get RTC time.");
    return dtToString(dt);
  }
  static String dtToString(DS1302_DateTime& dt) {
    return String(dt.dayWeek) + " " + String(dt.dayMonth) + "/"
            + String(dt.month) + "/" + String(dt.year) + " "
            + String(dt.hour) + ":" + String(dt.minute) + ":" + String(dt.second);
  }
};

// must instantiate static RTC members outside of the class.
RTC* RTC::globalRTC = nullptr;
bool RTC::running = false;

#endif //REDCROSS_INCLUDE_REALTIME_H
