/**
 * @file BarPAT.h
 *
 * @brief BarPAT class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_BARPAT_SRC_BARPAT_H_
#define REDCROSS_LIB_BARPAT_SRC_BARPAT_H_

#include <chrono>
#include <thread>
#include "I2CPerif.hpp"
//#include <Adafruit_MPL115A2.h>

namespace perif {

using namespace std;
using namespace std::chrono;

#define MPL115A2_DEFAULT_ADDRESS (0x60) /**< I2C address **/

/**< 10-bit Pressure ADC output value MSB **/
#define MPL115A2_REGISTER_PRESSURE_MSB (0x00)
/**< 10-bit Pressure ADC output value LSB **/
#define MPL115A2_REGISTER_PRESSURE_LSB (0x01)
/**< 10-bit Temperature ADC output value MSB **/
#define MPL115A2_REGISTER_TEMP_MSB (0x02)
/**< 10-bit Temperature ADC output value LSB **/
#define MPL115A2_REGISTER_TEMP_LSB (0x03)
#define MPL115A2_REGISTER_A0_COEFF_MSB (0x04)  /**< a0 coefficient MSB **/
#define MPL115A2_REGISTER_A0_COEFF_LSB (0x05)  /**< a0 coefficient LSB **/
#define MPL115A2_REGISTER_B1_COEFF_MSB (0x06)  /**< b1 coefficient MSB **/
#define MPL115A2_REGISTER_B1_COEFF_LSB (0x07)  /**< b1 coefficient LSB **/
#define MPL115A2_REGISTER_B2_COEFF_MSB (0x08)  /**< b2 coefficient MSB **/
#define MPL115A2_REGISTER_B2_COEFF_LSB (0x09)  /**< b2 coefficient LSB **/
#define MPL115A2_REGISTER_C12_COEFF_MSB (0x0A) /**< c12 coefficient MSB **/
#define MPL115A2_REGISTER_C12_COEFF_LSB (0x0B) /**< c12 coefficient LSB **/
/**< Start Pressure and Temperature Conversion **/
#define MPL115A2_REGISTER_STARTCONVERSION (0x12)

/**
 * A barometric pressure/temperature sensor.
 * TODO add altitude for upgraded mpl3115A2 sensor
 */
class BarPAT : public I2CPerif<> {
 private:
  float coeffs[4];
//  Adafruit_MPL115A2 mpl;
  // the barometric pressure
  float pressure;
  // the barometric temperature
  float temp;
  void updateData() override;
  void representData() override;
  bool readCoefficients();
  bool getPT(float &prssr, float &tmp);
 public:
  /**
   * Explicit value constructor for MPL115A2 sensor.
   *
   * @param address the I2C address
   */
  explicit BarPAT(uint8_t address = 0x60);
  bool begin() override;
};

}

#endif // REDCROSS_LIB_BARPAT_SRC_BARPAT_H_
