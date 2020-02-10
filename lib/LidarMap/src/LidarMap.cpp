//
// Created by tim on 2/9/20.
//

#include "LidarMap.h"

double LidarMap::getDistance(int count) {
  double distance = 0;
  bool biasCorrect = count == 1;
  for (int i = 0; i < count; ++i)
    distance += lidar.distance(biasCorrect || i == count-1);
  distance /= count;
  // Lidar distance is consistently ~10 cm off, so fix it.
  distance -= distance < 10 ? distance : 10;
  return distance;
}

String LidarMap::name() {
  return "Lidar";
}

double LidarMap::getData() {
  return getDistance(100);
}
String LidarMap::dataAsString(const double &data) {
  return String(data);
}
