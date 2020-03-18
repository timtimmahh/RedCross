//
// Created by tim on 2/9/20.
//

#ifndef REDCROSS_LIB_LIDARMAP_H
#define REDCROSS_LIB_LIDARMAP_H

#include "peripheral/I2CSensor.hpp"
#include <cstdint>
#include <LIDARLite.h>

namespace perif {
  /**
   * Wrapper around the Garmin LIDAR-Lite library.
   */
  class LidarMap : public I2CSensor0 {
  private:
	LIDARLite lidar;
	// the distance in cm
	double distance;
	bool begin() override;
	/**
	 * Obtains distance data in centimeters.
	 */
	void updateData() override;
	/**
	 * Converts distance data to a String.
	 */
	void representData() override;
  public:
	/**
	 * Constructor for Lidar. Sets up Lidar immediately.
	 */
	explicit LidarMap(uint8_t address = 0x62);
	/**
	 * Gets the distance (in cm) count times and returns the average.
	 */
	double getDistance(uint8_t count = 1);
  };
}
#endif //REDCROSS_LIB_LIDARMAP_H
