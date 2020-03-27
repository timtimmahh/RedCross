/**
 * @file Thread.hpp
 *
 * @brief Thread class declaration.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_MAIN_IPC_THREAD_HPP_
#define REDCROSS_MAIN_IPC_THREAD_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_pthread.h>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <utility>
#include "sdkconfig.h"

using namespace std;

/**
 * A wrapper around std::thread to simplify thread creation and destruction
 * as well as manage events through interrupts with thread-safety.
 */
class Thread {
 private:
  const string taskName;
  thread _thread;
  const uint32_t stackSize;
  const uint32_t priority;
  const int affinity;
  atomic_bool started{false};
  mutex startMutex{};
  condition_variable startCond{};

  /**
   * The thread's callable. Notifies calling thread that this thread has
   * started so caller can continue, then runs the main task in a loop.
   */
  [[noreturn]] void exec() {
	// allow caller to continue now that this has started
	if (!started) {
	  started = true;
	  unique_lock<mutex> lock(startMutex);
	  startCond.notify_all();
	}

	// perform any initialization
	init();

	// run the loop function
	while (true) {
	  loop();
	}
  }
 protected:
  virtual void init() {}
  /**
   * Performs a task in a loop.
   */
  virtual void loop() {}
 public:
  /**
   * Initializes a thread with the initialization function, task name, stack
   * size, priority, and core to pin to.
   *
   * @param init the initialization function
   * @param taskName the task name
   * @param stackSize the stack size
   * @param priority the priority
   * @param appCore the core to pin to
   */
  explicit Thread(string taskName = CONFIG_PTHREAD_TASK_NAME_DEFAULT,
				  uint32_t stackSize = CONFIG_MAIN_TASK_STACK_SIZE,
				  uint32_t priority = CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
				  int appCore = CONFIG_PTHREAD_TASK_CORE_DEFAULT)
	  : taskName(move(taskName)),
		stackSize(stackSize),
		priority(priority),
		affinity(appCore) {}
  /**
   * Creates a thread without any initialization and all default configurations.
   */
  Thread() : Thread(nullptr) {}

  /**
   * Start the thread and wait until it has started and initialized.
   */
  void start() {
	// prevent starting multiple times
	if (started)
	  return;
	unique_lock<mutex> startGuard(startMutex);

	// setup the esp pthread configuration for this thread
	esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
	cfg.stack_size = stackSize;
	cfg.prio = priority;
	cfg.thread_name = taskName.c_str();
	cfg.pin_to_core = affinity;
	esp_pthread_set_cfg(&cfg);

	// create the actual thread
	_thread = thread([this]() { exec(); });

	// wait until the thread has started successfully
	startCond.wait(startGuard, [this]() {
	  return started.load();
	});
  }
};

#endif // REDCROSS_MAIN_IPC_THREAD_HPP_
