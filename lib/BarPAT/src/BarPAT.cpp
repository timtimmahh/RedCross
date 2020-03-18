/**
 * @file BarPat.cpp
 *
 * @brief BarPAT class declaration.
 *
 * This work complies with the JMU honor code.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "BarPAT.h"

perif::BarPAT::BarPAT(uint8_t address)
	: I2CSensor("BarPAT", address),
	  mpl(),
	  pressure(0.0f),
	  temp(0.0f) {
}

bool perif::BarPAT::begin() {
  bool res = I2CSensor::begin();
  mpl.begin(address, &wire);
  return res && isConnected();
}

void perif::BarPAT::updateData() {
  float press = 0, t = 0;
  mpl.getPT((float*)&press, (float*)&t);
  this->pressure = (float)press;
  this->temp = (float)t;
}

void perif::BarPAT::representData() {
  data["pressure"] = pressure;
  data["temp"] = temp;
}
