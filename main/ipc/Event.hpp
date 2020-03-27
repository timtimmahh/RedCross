/**
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

template<typename... Args>
class EventListener {
  virtual void onEvent(Args... args) = 0;
};

template<typename T>
class Event {
 private:
  string eventName;
  Queue<T> data;
  Queue<pair<thread *, function<void()>>> observers;

 public:
  static unordered_map<string, Event *> events;
  Event(const char *base, const char *name)
	  : eventName(string(base) + "_" + name) {
	events[eventName] = this;
  }

};

//unordered_map<string, Event *> Event::events = unordered_map<string, Event *
//>();

#endif // REDCROSS_LIB_IPC_EVENT_HPP_
