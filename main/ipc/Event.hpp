/**
 * @dir main/ipc/
 *
 * @file Event.hpp
 *
 * @brief Event class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_IPC_EVENT_HPP_
#define REDCROSS_LIB_IPC_EVENT_HPP_

#include "esp_event.h"
#include "Queue.hpp"
#include <thread>
#include <unordered_map>
#include <string>

using namespace std;

/**
 * An interface for event callbacks.
 *
 * @tparam Args argument types for the callback
 */
template<typename... Args>
class EventListener {
  /**
   * The function to implement to receive event callbacks.
   *
   * @param args the callback arguments
   */
  virtual void onEvent(Args... args) = 0;
};

/**
 * An event instance to register for notifications.
 */
class Event {
 private:
  /**
   * The identifier for this event.
   */
  string eventName;
  /**
   * Any necessary event data.
   */
  Queue<uint32_t> data;
  /**
   * Registered event callbacks.
   */
  Queue<pair<thread *, function<void()>>> observers;

 public:
  /**
   * All initialized event instances to be notified.
   */
  static unordered_map<string, Event *> events;
  /**
   * Creates an event type with the ESP base and ID values.
   *
   * @param base the ESP event base name
   * @param name the ESP event base id
   */
  Event(const char *base, const char *name)
	  : eventName(string(base) + "_" + name) {
	events[eventName] = this;
  }

};


unordered_map<string, Event *> Event::events = unordered_map<string, Event *
>();

#endif // REDCROSS_LIB_IPC_EVENT_HPP_
