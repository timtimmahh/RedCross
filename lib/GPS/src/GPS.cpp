//
// Created by tim on 2/25/20.
//

#include "GPS.h"

namespace perif {
GPS::GPS(uint8_t updateInterval)
	: UARTSensor("GPS", Serial2),
	  gps(&Serial2),
	  updateInterval(updateInterval),
	  lastUpdate(millis()) {
}

bool GPS::begin() {
  bool res = gps.begin(9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);
  gps.sendCommand(PGCMD_ANTENNA);
  delay(1000);
  gps.println(PMTK_Q_RELEASE);
  return res;
}

void GPS::updateData() {
  updated = false;
  char c = gps.read();
  if (c)
	Serial.println(c);

  // if a sentence is received, we can check the checksum, parse it...
  if (gps.newNMEAreceived()) {
	// a tricky thing here is if we print the NMEA sentence, or data
	// we end up not listening and catching other sentences!
	// so be very wary if using OUTPUT_ALLDATA and trytng to print out data
	Serial.println(gps.lastNMEA());   // this also sets the newNMEAreceived()
	// flag to false
	updated = gps.parse(gps.lastNMEA()); // we can fail to parse a sentence
	// in which case we should just wait for another
  }
}

void GPS::representData() {
  // wait to update to prevent unnecessary updates
  if (updated && millis() - lastUpdate > updateInterval * 1000) {
	lastUpdate = millis();
	PerifBase::data["hour"] =
		(gps.hour < 10 ? "0" : "") + to_string(gps.hour);
	PerifBase::data["minute"] = (gps.minute < 10 ? "0" : "")
		+ to_string(gps.minute);
	PerifBase::data["seconds"] = (gps.seconds < 10 ? "0" : "")
		+ to_string(gps.seconds);
	PerifBase::data["milliseconds"] = (gps.milliseconds < 10
									   ? "00" : (gps.milliseconds < 100
												 ? "0" : ""))
		+ to_string(gps.milliseconds);
	PerifBase::data["day"] = gps.day;
	PerifBase::data["month"] = gps.month;
	PerifBase::data["year"] = gps.year;
	PerifBase::data["hasFix"] = (int)gps.fix;
	PerifBase::data["fixQuality"] = (int)gps.fixquality;
	if (gps.fix) {
	  PerifBase::data["latitude"] = to_string(gps.latitude, 4);
	  PerifBase::data["lat"] = gps.lat;
	  PerifBase::data["longitude"] = to_string(gps.longitude, 4);
	  PerifBase::data["lon"] = gps.lon;
	  PerifBase::data["speed"] = gps.speed;
	  PerifBase::data["angle"] = gps.angle;
	  PerifBase::data["altitude"] = gps.altitude;
	  PerifBase::data["satellites"] = (int)gps.satellites;
	}
  }
}
}