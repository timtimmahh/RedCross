//
// Created by tim on 2/25/20.
//

#include "AccelGyro.h"

namespace perif {

AccelGyro::AccelGyro(byte address)
	: I2CSensor("AccelGyro", address),
	mpu(address),
	data{0, 0, 0, 0, 0, 0},
	lastUpdate(0) {
}

bool AccelGyro::begin() {
  // start the I2C wire
  bool wireRes = I2CSensor::begin();
  // perform mpu6050 initialization
  mpu.initialize();
  return wireRes && mpu.testConnection();
}

const int16_t *AccelGyro::getAcceleration() {
  mpu.getAcceleration(data,
  	&data[1],
  	&data[2]);
  return data;
}

const int16_t *AccelGyro::getRotation() {
  mpu.getRotation(&data[3],
  	&data[4],
  	&data[5]);
  return data;
}

void AccelGyro::updateData() {
  unsigned long us = micros();
  // TODO asynchronous delay
  if (us - lastUpdate < US_DELAY)
    delayMicroseconds(US_DELAY - (us - lastUpdate));
  mpu.getMotion6(
	  data,
	  &data[1],
	  &data[2],
	  &data[3],
	  &data[4],
	  &data[5]);
  lastUpdate = micros();
}

void AccelGyro::representData() {
  PerifBase::data["ax"] = to_string(data[0]);
  PerifBase::data["ay"] = to_string(data[1]);
  PerifBase::data["az"] = to_string(data[2]);
  PerifBase::data["gx"] = to_string(data[3]);
  PerifBase::data["gy"] = to_string(data[4]);
  PerifBase::data["gz"] = to_string(data[5]);
}
}