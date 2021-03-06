/**
 * @dir main/perif/
 * @file Perif.hpp
 *
 * @brief PerifBase and Perif class definitions.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_INCLUDE_SENSOR_H
#define REDCROSS_INCLUDE_SENSOR_H

#include <cstdint>
#include <functional>
#include <map>
#include <list>
#include <string>
#include "timer/Timer.hpp"
#include "utils.h"

using namespace std;

/**
 * Basic type alias for device mappings.
 */
using DevMap = std::map<std::string, std::string>;

namespace perif {

/**
 * Base class for peripherals.
 */
class PerifBase {
 public:
  /**
   * The key-value mapping of peripheral data.
   */
  DevMap data;
  /**
   * Hooks registered to this peripheral so observers can be notified of any
   * necessary changes.
   */
  list<function<void(DevMap&)>> hooks;
  /**
   * Explicit Value Constructor.
   *
   * @param name the peripheral's name
   */
  explicit PerifBase(const char *name)
	  : data(), hooks() {
	data["name"] = name;
  }
  /**
   * Copy Constructor.
   *
   * @param original the original instance
   */
  PerifBase(const PerifBase &original) = default;
  /**
   * Registers a hook for obtaining sensor data on update in Json format.
   *
   * @param hook the observer function
   */
  void registerHook(function<void(DevMap&)>&& hook) {
	hooks.push_back(hook);
  }
};


/**
 * Abstract class for sensors. Used to manage all peripheral data and
 * persistence.
 */
class Perif : public PerifBase {
 protected:
  /**
   * A timer for managing peripheral updates.
   */
  Timer timer;
  /**
   * Updates sensor data. Subclasses should maintain a reference to the data
   * obtained from this function so it can be provided to observers.
   */
  virtual void updateData() = 0;
  /**
   * The representation of this peripheral's data in a meaningful way for
   * debugging, caching, and database operations.
   */
  virtual void representData() = 0;
  /**
   * Obtains a reference to this peripheral's timer.
   *
   * @return a timer reference
   */
  Timer &getTimer() { return timer; }
 public:
  /**
   * Explicit Value Constructor taking the name of this sensor.
   *
   * @param name this sensor's name
   */
  explicit Perif(const char *name)
	  : PerifBase(name), timer() {
  }
  /**
   * Copy Constructor.
   *
   * @param original the original instance
   */
  Perif(const Perif &original) = default;
  /**
   * Destructor.
   */
  ~Perif() = default;
  /**
   * Performs initialization of a peripheral. Necessary to avoid
   * initialization in the constructor which causes many side effects.
   *
   * @return whether the peripheral initialization succeeded
   */
  virtual bool begin() = 0;
  /**
   * Returns whether the sensor is connected or not.
   *
   * @return if the sensor is connected
   */
  virtual bool isConnected() = 0;
  /**
   * Performs a manual update of peripheral data and notifies observers.
   */
  void update() {
	// performs a manual data retrieval
	updateData();
	// update the data mapping so hooks can process it
	representData();
	// notify all observers
	for (auto& hook : hooks)
	  hook(data);
  }
};

} // namespace perif

#endif //REDCROSS_INCLUDE_SENSOR_H
