/**
 * @file LidarMap.cpp
 *
 * @brief LidarMap class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "LidarMap.h"

namespace perif {
LidarMap::LidarMap(uint8_t address)
	: I2CPerif("WaterLevel", address),
	  distance(0.0) {
}

double LidarMap::measure(bool biasCorrection) {
  writeByte(address, 0x00, biasCorrection ? 0x04 : 0x03);
  uint8_t distArr[2];
  readBytes(address, 0x8f, 2, distArr);
  return (distArr[0] << 8u) + distArr[1];
}

double LidarMap::getDistance(uint8_t count) {
  double data = 0;

  for (int i = 0; i < count; ++i)
	data += measure(false);

  data /= count;
  // Lidar distance is consistently ~10 cm off, so fix it.
  data -= 8.5;
  return max(0.0, data);
}

void LidarMap::updateData() {
  distance = getDistance(100);
}

void LidarMap::representData() {
  data["distance"] = to_string(distance, 2);
}

template<int v1, int v2, int v3>
bool LidarMap::configure() {
  return writeByte(address, 0x02, v1)
	  && writeByte(address, 0x04, v2)
	  && writeByte(address, 0x1c, v3);
}

bool LidarMap::begin() {
  return I2CPerif::begin() && doConfig();
}

void LidarMap::reset() {
  writeByte(address, 0x00, 0x00);
}
}