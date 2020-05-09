/**
 * @dir main/include/
 *
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

/**
 * Wrapper around ESP_ERR to simplify usage and reduce boilerplate code.
 */
class Err {
 public:
  /**
   * The main error flags.
   */
  enum Flag : unsigned {
	ErrFAIL = 0x000,			// ESP Failure
	ErrOK = 0x001,				// ESP Success
	NO_MEM = 0x002,				// ESP non memory
	INVALID_ARG = 0x004,		// ESP bad argument passeed
	INVALID_STATE = 0x008,		// ESP bad state
	INVALID_SIZE = 0x010,		// ESP wrong array size
	NOT_FOUND = 0x020,			// ESP not found
	NOT_SUPPORTED = 0x040,		// ESP operation not supported
	TIMEOUT = 0x080,			// ESP timeout
	INVALID_RESPONSE = 0x100,	// ESP bad response
	INVALID_CRC = 0x200,		// ESP bad CRC
	INVALID_VERSION = 0x400,	// ESP wrong version
	INVALID_MAC = 0x800			// ESP bad MAC
  };

  /**
   * Creates an error directly from a Flag.
   *
   * @param value the Flag
   */
  Err(Flag value) : value(value) {}
  /**
   * Creates an error from a C-style esp_err_t.
   * @param err the ESP error
   * @param valid bitwise OR-ed Flags indicating a valid response
   */
  Err(esp_err_t err, Flag valid = ErrOK) : Err(convert(err, valid)) {}

  /**
   * Converts this error to uint16_t.
   *
   * @return this value as uint16_t
   */
  inline operator uint16_t() const {
	return value;
  }
  /**
   * Converts this error to a boolean for easy checking.
   *
   * @return whether this is an error or not
   */
  inline operator bool() const {
	return static_cast<bool>(value);
  }
  /**
   * Converts an esp_err_t to a Flag.
   *
   * @param err the original ESP error
   * @param valid bitwise OR-ed valid Flags
   * @return the Flag representation of esp_err_t
   */
  static inline Flag convert(esp_err_t err, Flag valid = ErrOK) {
	return static_cast<Flag>((err != ESP_FAIL
							  ? 0x1u << (static_cast<unsigned>(err) & 0xFu)
							  : Err::ErrFAIL) & valid);
  }
 private:
  /**
   * This error's flag value.
   */
  Flag value;
};

/**
 * Performs error checking on C-style ESP operations.
 *
 * @tparam valid valid error flags
 */
template<Err::Flag valid = Err::ErrOK>
class ErrorHandler {
 public:
  /**
   * Performs the operations and ensures all have valid results.
   *
   * @return whether all functions ran successfully
   */
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
  /**
   * A list of ESP function to run.
   */
  list<pair<const string, function<esp_err_t()>>> espFuncs;
  /**
   * Used for logging to determine the source of the function call.
   */
  string callerName;
  /**
   * An optional label to log the function call name.
   */
  const char *gLabel;
  /**
   * Whether the functions have been run and checked.
   */
  bool ranCheck = false;
  /**
   * Logs ESP functions if its result is invalid.
   *
   * @param label the logging tag
   * @param error the error
   * @return whether there was an error
   */
  bool logE(const string &label, Err &&error) {
	if (!error) {
	  string tag = (callerName + "::" + label);
	  ESP_LOGE(tag.c_str(), "error = %d", static_cast<uint16_t>(error));
	  return false;
	}
	return true;
  }
 public:
  /**
   * Creates an instance this handler to perform ESP operations safely.
   *
   * @param caller the calling function (for debugging)
   * @param globalLabel a default label to use if a function doesn't have one
   */
  explicit ErrorHandler(string &&caller,
						const char *globalLabel = nullptr)
	  : espFuncs(), callerName(caller), gLabel(globalLabel) {}
  /**
   * Function call operator to pass an ESP function, it's name and any
   * arguments.
   *
   * @tparam Args the ESP function args
   * @param espFunc the ESP function
   * @param label the function label (for debugging)
   * @param args the ESP function argument values
   * @return this handler for call chaining
   */
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

/**
 * Function call operator to pass an ESP function, it's name and any
 * arguments.
 *
 * @tparam Ret the ESP function return type
 * @tparam Args the ESP function args
 * @tparam func the ESP function signature
 * @param idfFunc the ESP function
 * @param valid the function label (for debugging)
 * @return a wrapper around the ESP function with error checking
 */
template<typename Ret, typename... Args, template<Ret(*)(Args...), Args...>
	class... func>
function<bool(Args...)> _(esp_err_t idfFunc(Args...),
						  Err::Flag valid = Err::ErrOK) {
  return [&idfFunc, &valid](Args... args) -> bool {
	return Err::convert(idfFunc(args...), valid);
  };
}

#endif //REDCROSS_MAIN_INCLUDE_ERRORLOG_HPP_
