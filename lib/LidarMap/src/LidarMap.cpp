//
// Created by tim on 2/25/20.
//

#include "LidarMap.h"

namespace perif {
LidarMap::LidarMap(uint8_t address)
	: I2CSensor("LidarMap", address),
	  lidar(),
	  distance(0.0) {
}

bool LidarMap::begin() {
  bool res = I2CSensor::begin();
  lidar.configure(0, address);
  return res;
}

double LidarMap::getDistance(uint8_t count) {
  double data = 0;
  bool biasCorrect = count == 1;
  for (int i = 0; i < count; ++i)
	data += lidar.distance(biasCorrect || i == count - 1);
  data /= count;
  // Lidar distance is consistently ~10 cm off, so fix it.
  data -= 10;
  return max(0.0, data);
}

void LidarMap::updateData() {
  distance = getDistance(100);
}

void LidarMap::representData() {
  data["distance"] = to_string(distance, 2);
}
}