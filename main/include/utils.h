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

#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define CONCAT(a, b) a ## b
#define CONCAT2(a, b) CONCAT(a, b)

#define LINE0()
#define LINE(op) op;
#define LINE1(op) LINE(op)
#define LINE2(op, op1) LINE(op) LINE(op1)
#define LINE3(op, op1, op2) LINE2(op, op1) LINE(op2)

#define LOG(msgs...) log(fileToTag(__FILE__), msgs)

inline string fileToTag(const char *file) {
  string tagStr = file;
  unsigned index = tagStr.find_last_of('/');
  if (index != string::npos)
	tagStr = tagStr.substr(index + 1);
  return tagStr;
}

template<typename StrType>
void log(const string &tag, StrType str) {
  stringstream ss;
  ss << str;
  ESP_LOGI(tag.c_str(), "%s", ss.str().c_str());
}

template<typename StrType>
void apnd(const string &tag, stringstream &ss, StrType str) {
  ss << str;
  ESP_LOGI(tag.c_str(), "%s", ss.str().c_str());
}

template<typename StrType, typename... StrTypes>
void apnd(const string &tag, stringstream &ss, StrType str, StrTypes... msgs) {
  ss << str;
  apnd(tag, ss, msgs...);
}

template<typename StrType, typename... StrTypes>
void log(const string &tag, StrType str, StrTypes... msg) {
  stringstream ss;
  apnd(tag, ss, str, msg...);
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
  string val = std::to_string(rnd(fp * scalar) / scalar);
  return val.substr(0, val.find('.') + precision + 1);
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
