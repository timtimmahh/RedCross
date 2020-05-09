/**
 * @dir main/
 *
 * @file main.cpp
 *
 * @brief program entry file.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#include "redcross.h"
#include <thread>

using namespace std::chrono;
/**
 * Entry function for ESP32 framework.
 */
extern "C" void app_main() {
  try {
	App().start();
  } catch (const runtime_error &e) {
	LOG("Exception caught: ", e.what());
  }
}
