/**
 * @dir main/devices/BarPAT/
 *
 * @file BarPAT.cpp
 *
 * @brief BarPAT class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "BarPAT.h"

perif::BarPAT::BarPAT(uint8_t address)
	: I2CPerif("BarPAT", address),
	  coeffs{0.0F, 0.0F, 0.0F, 0.0F},
	  pressure(0.0f),
	  temp(0.0f) {
}

bool perif::BarPAT::begin() {
//  mpl.begin(address, &wire);
  return I2CPerif::begin() && readCoefficients();
}

void perif::BarPAT::updateData() {
  float press = 0, t = 0;
  getPT(press, t);
  this->pressure = press;
  this->temp = t;
}

void perif::BarPAT::representData() {
  data["pressure"] = to_string(pressure, 2);
  data["temp"] = to_string(temp, 2);
}

bool perif::BarPAT::readCoefficients() {
  bool res;
  uint8_t data[8];
  uint16_t tmpCoeffs[4];
  res = readBytes(address, MPL115A2_REGISTER_A0_COEFF_MSB, 8, data);
  for (int i = 0; i < 4; ++i)
	tmpCoeffs[i]
		= (static_cast<uint16_t>(static_cast<uint16_t>(data[2 * i]) << 8u)
		| data[(i + 1) * 2]);
  tmpCoeffs[3] = tmpCoeffs[3] >> 2u;
  this->coeffs[0] = static_cast<float>(tmpCoeffs[0]) / 8;
  this->coeffs[1] = static_cast<float>(tmpCoeffs[1]) / 8192;
  this->coeffs[2] = static_cast<float>(tmpCoeffs[2]) / 16384;
  this->coeffs[3] = static_cast<float>(tmpCoeffs[3]) / 4194304.0f;
  return res;
}
bool perif::BarPAT::getPT(float &prssr, float &tmp) {
  bool res = writeByte(address, MPL115A2_REGISTER_STARTCONVERSION, 0x00);
  std::this_thread::sleep_for(milliseconds(5));
  uint8_t data[4];
  uint16_t conv[2];
  res = res && readBytes(address, MPL115A2_REGISTER_PRESSURE_MSB, 4, data);
  for (int i = 0; i < 2; ++i)
	conv[i] = static_cast<uint16_t>(static_cast<uint16_t>(
		static_cast<uint16_t>(data[i * 2]) << 8u) | data[(i + 1) * 2]
	) >> 6u;

  prssr = coeffs[0] + (coeffs[1] + coeffs[3] * static_cast<float>(conv[1]))
	  * static_cast<float>(conv[0]) + (coeffs[2] * static_cast<float>(conv[1]));
  prssr = ((65.0F / 1023.0F) * prssr) + 50.0F;
  tmp = (static_cast<float>(conv[1]) - 498.0F) / -5.35F + 25.0F;
  return res;
}
