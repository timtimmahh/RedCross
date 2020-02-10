//
// Created by tim on 2/2/20.
//

#include "AccelGyro.h"



Coords AccelGyro::getAcceleration() {
  mpu.getAcceleration(&data[0], &data[1], &data[2]);
  return data;
}

Coords AccelGyro::getRotation() {
  mpu.getRotation(&data[3], &data[4], &data[5]);
  return data;
}

String AccelGyro::name() {
  return "AccelGyro";
}

Coords AccelGyro::getData() {
  start();
  mpu.getMotion6(
      &data[0], &data[1],&data[2],
      &data[3],&data[4],&data[5]);
  return data;
}

String AccelGyro::dataAsString(const Coords& cData) {
  return cData.toString();
}
