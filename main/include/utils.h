/**
 * @file utils.h
 *
 * @brief Global utilities.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_INCLUDE_UTILS_H_
#define REDCROSS_INCLUDE_UTILS_H_

#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <esp_log.h>

using namespace std;

template<typename StrType>
void log(StrType str) {
  stringstream ss;
  ss << str;
  ESP_LOGI(__FILE__, "%s", ss.str().c_str());
}

template<typename StrType>
void apnd(stringstream &ss, StrType str) {
  ss << str;
  ESP_LOGI(__FILE__, "%s", ss.str().c_str());
}

template<typename StrType, typename... StrTypes>
void apnd(stringstream &ss, StrType str, StrTypes... msgs) {
  ss << str;
  apnd(ss, msgs...);
}

template<typename StrType, typename... StrTypes>
void log(StrType str, StrTypes... msg) {
  stringstream ss;
  apnd(ss, str, msg...);
}

/**
 * Copies at min(SourceLen, size) bytes from source to target buffer.
 *
 */
template<typename InputIt, typename SourceLen, typename T, size_t size>
void copy_min_to_buffer(InputIt source,
						SourceLen source_length,
						T (&target)[size]) {
  copy_n(source, min(static_cast<size_t>(source_length), size), target);
}

template<typename T, size_t size>
void copy_min_to_buffer(string &&source, T (&target)[size]) {
  copy_n(source.begin(),
		 min(static_cast<size_t>(source.length()), size), target);
}

/**
 * Helper to create a string representation of a floating point number with a
 * certain number of decimal point precision.
 *
 * @tparam FP the type of floating point number
 * @tparam rnd the rounding function
 * @param fp the floating point number
 * @param precision the number of decimal points
 * @return a string representation
 */
template<typename FP, FP(*rnd)(FP)>
inline std::string to_string(FP fp, uint8_t precision = 2) {
  unsigned long scalar = pow(10, precision);
  return std::to_string(rnd(fp * scalar) / scalar);
}

/**
 * Helper to convert a float to a string with a certain number of decimal
 * points.
 *
 * @param fp the number
 * @param precision the decimal points
 * @return the string representation
 */
inline std::string to_string(float fp, uint8_t precision = 2) {
  return to_string<float, roundf>(fp, precision);
}

/**
 * Helper to convert a double to a string with a certain number of decimal
 * points.
 *
 * @param fp the number
 * @param precision the decimal points
 * @return the string representation
 */
inline std::string to_string(double fp, uint8_t precision = 2) {
  return to_string<double, round>(fp, precision);
}

#endif //REDCROSS_INCLUDE_UTILS_H_
