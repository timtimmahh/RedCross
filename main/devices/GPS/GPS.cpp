/**
 * @file GPS.cpp
 *
 * @brief GPS class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "GPS.h"

namespace perif {
GPS::GPS(uint8_t updateInterval)
	: UARTPerif("GPS"),
	  updateInterval(updateInterval) {
}

bool GPS::begin() {
//  bool res = gps.begin(9600);
//  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
//  gps.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);
//  gps.sendCommand(PGCMD_ANTENNA);
//  delay(1000);
//  gps.println(PMTK_Q_RELEASE);
//  return res;
  return UARTPerif::begin();
}

void GPS::updateData() {
  updated = false;
//  char c = gps.read();
//  if (c)
//	Serial.println(c);

  // if a sentence is received, we can check the checksum, parse it...
//  if (gps.newNMEAreceived()) {
  // a tricky thing here is if we print the NMEA sentence, or data
  // we end up not listening and catching other sentences!
  // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
//	Serial.println(gps.lastNMEA());   // this also sets the newNMEAreceived()
  // flag to false
//	updated = gps.parse(gps.lastNMEA()); // we can fail to parse a sentence
  // in which case we should just wait for another
//  }
}

void GPS::representData() {
  // wait to update to prevent unnecessary updates
//  if (updated && millis() - lastUpdate > updateInterval * 1000) {
//	lastUpdate = millis();
//    (gps.hour < 10 ? "0" : "") +
//    (gps.minute < 10 ? "0" : "")
//        +
//    (gps.seconds < 10 ? "0" : "")
//        +
//    (gps.milliseconds < 10
//     ? "00" : (gps.milliseconds < 100
//               ? "0" : ""))
//        +
  /*PerifBase::data["hour"] = to_string(gps.hour);
  PerifBase::data["minute"] = to_string(gps.minute);
  PerifBase::data["seconds"] = to_string(gps.seconds);
  PerifBase::data["milliseconds"] = to_string(gps.milliseconds);
  PerifBase::data["day"] = to_string(gps.day);
  PerifBase::data["month"] = to_string(gps.month);
  PerifBase::data["year"] = to_string(gps.year);
  PerifBase::data["hasFix"] = to_string((int)gps.fix);
  PerifBase::data["fixQuality"] = to_string((int)gps.fixquality);
  if (gps.fix) {
	PerifBase::data["latitude"] = to_string(gps.latitude, 4);
	PerifBase::data["lat"] = gps.lat;
	PerifBase::data["longitude"] = to_string(gps.longitude, 4);
	PerifBase::data["lon"] = gps.lon;
	PerifBase::data["speed"] = to_string(gps.speed, 4);
	PerifBase::data["angle"] = to_string(gps.angle, 4);
	PerifBase::data["altitude"] = to_string(gps.altitude, 4);
	PerifBase::data["satellites"] = to_string((int)gps.satellites);
  }*/
//  }
}
}