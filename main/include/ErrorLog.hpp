/**
 * @file ErrorLog.hpp
 *
 * @brief ErrorLog class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_INCLUDE_ERRORLOG_HPP_
#define REDCROSS_MAIN_INCLUDE_ERRORLOG_HPP_

#include "utils.h"
#include <esp_log.h>
#include <esp_err.h>
#include <cstdio>
#include <string>
#include <list>
#include <functional>

using namespace std;

class Err {
 public:
  enum Flag : unsigned {
	ErrFAIL = 0x000,
	ErrOK = 0x001,
	NO_MEM = 0x002,
	INVALID_ARG = 0x004,
	INVALID_STATE = 0x008,
	INVALID_SIZE = 0x010,
	NOT_FOUND = 0x020,
	NOT_SUPPORTED = 0x040,
	TIMEOUT = 0x080,
	INVALID_RESPONSE = 0x100,
	INVALID_CRC = 0x200,
	INVALID_VERSION = 0x400,
	INVALID_MAC = 0x800
  };

  Err(Flag value) : value(value) {}
  Err(esp_err_t err, Flag valid = ErrOK) : Err(convert(err, valid)) {}

  inline operator uint16_t() const {
	return value;
  }

  inline operator bool() const {
	return static_cast<bool>(value);
  }
  static inline Flag convert(esp_err_t err, Flag valid = ErrOK) {
	return static_cast<Flag>((err != ESP_FAIL
							  ? 0x1u << (static_cast<unsigned>(err) & 0xFu)
							  : Err::ErrFAIL) & valid);
  }
 private:
  Flag value;
};

template<Err::Flag valid = Err::ErrOK>
class ErrorHandler {
 public:
  bool runCheck() {
	size_t passed = 0;
	unsigned size = espFuncs.size();
	for (auto &espFunc : espFuncs) {
	  if (logE(espFunc.first, Err(espFunc.second(), valid)))
		passed++;
	}
	espFuncs.clear();
	ranCheck = true;
	return passed == size;
  }
 private:
  list<pair<const string, function<esp_err_t()>>> espFuncs;
  string callerName;
  const char *gLabel;
  bool ranCheck = false;
  bool logE(const string &label, Err &&error) {
	if (!error) {
	  string tag = (callerName + "::" + label);
	  ESP_LOGE(tag.c_str(), "error = %d", static_cast<uint16_t>(error));
	  return false;
	}
	return true;
  }
 public:
  explicit ErrorHandler(string &&caller,
						const char *globalLabel = nullptr)
	  : espFuncs(), callerName(caller), gLabel(globalLabel) {}
  template<typename... Args>
  ErrorHandler<valid> &operator()(esp_err_t espFunc(Args...),
								  string label,
								  Args... args) {

	espFuncs.push_back(
		make_pair(label, [&espFunc, args...]() {
		  return espFunc(args...);
		}));
	return *this;
  }
};

template<typename Ret, typename... Args, template<Ret(*)(Args...), Args...>
	class... func>
function<bool(Args...)> _(esp_err_t idfFunc(Args...),
						  Err::Flag valid = Err::ErrOK) {
  return [&idfFunc, &valid](Args... args) -> bool {
	return Err::convert(idfFunc(args...), valid);
  };
}

#endif //REDCROSS_MAIN_INCLUDE_ERRORLOG_HPP_
