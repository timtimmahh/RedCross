/**
 * @file utils.h
 *
 * @brief utils class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_INCLUDE_UTILS_H_
#define REDCROSS_INCLUDE_UTILS_H_

#include <cmath>
#include <string>

template<typename FP, FP(*rnd)(FP)>
inline std::string to_string(FP fp, uint8_t precision = 2) {
  unsigned long scalar = pow(10, precision);
  return std::to_string(rnd(fp * scalar) / scalar);
}

inline std::string to_string(float fp, uint8_t precision = 2) {
  return to_string<float, roundf>(fp, precision);
}

inline std::string to_string(double fp, uint8_t precision = 2) {
  return to_string<double, round>(fp, precision);
}


#endif //REDCROSS_INCLUDE_UTILS_H_
