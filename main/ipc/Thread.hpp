/**
 * @dir main/ipc/
 *
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
  /**
   * The name of this thread task.
   */
  const string taskName;
  /**
   * The actual std::thread behind this.
   */
  thread _thread;
  /**
   * The stack size of the thread.
   */
  const uint32_t stackSize;
  /**
   * The thread priority.
   */
  const uint32_t priority;
  /**
   * Which CPU to run on. (PRO, APP, or both)
   */
  const int affinity;
  /**
   * Whether this thread should be attached to its calling thread.
   */
  const bool attached;
  /**
   * Whether this thread has started.
   */
  atomic_bool started{false};
//  mutex startMutex{};
//  condition_variable startCond{};

  /**
   * The thread's callable. Notifies calling thread that this thread has
   * started so caller can continue, then runs the main task in a loop.
   */
  [[noreturn]] void exec() {
	// allow caller to continue now that this has started
	if (!attached && !started) {
	  started = true;
//	  unique_lock<mutex> lock(startMutex);
//	  startCond.notify_all();
	}

	// perform any initialization
	init();

	// run the loop function
	while (true) {
	  loop();
	}
  }
 protected:
  /**
   * Performs any initialization once the new thread starts running.
   */
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
   * @param taskName the task name
   * @param stackSize the stack size
   * @param priority the priority
   * @param appCore the core to pin to
   * @param attach if this thread should attach to caller
   */
  explicit Thread(string taskName = CONFIG_PTHREAD_TASK_NAME_DEFAULT,
				  uint32_t stackSize = CONFIG_MAIN_TASK_STACK_SIZE,
				  uint32_t priority = CONFIG_PTHREAD_TASK_PRIO_DEFAULT,
				  int appCore = CONFIG_PTHREAD_TASK_CORE_DEFAULT,
				  bool attach = false)
	  : taskName(move(taskName)),
		stackSize(stackSize),
		priority(priority),
		affinity(appCore),
		attached(attach) {}

  /**
   * Start the thread and wait until it has started and initialized.
   */
  void start() {
	// prevent starting multiple times
	if (started && !attached)
	  return;

	if (attached) exec();
	else {
//	unique_lock<mutex> startGuard(startMutex);

	  // setup the esp pthread configuration for this thread
	  esp_pthread_cfg_t cfg = esp_pthread_get_default_config();
	  cfg.stack_size = stackSize;
	  cfg.prio = priority;
	  cfg.thread_name = taskName.c_str();
	  cfg.pin_to_core = affinity;
	  esp_pthread_set_cfg(&cfg);

	  try {

		// create the actual thread
		_thread = thread([this]() { exec(); });

		// wait until the thread has started successfully
//	  startCond.wait(startGuard, [this]() {
//		return started.load();
//	  });
	  } catch (const runtime_error &e) {
		LOG("Exception caught: ", e.what());
	  }
	}
  }
};

#endif // REDCROSS_MAIN_IPC_THREAD_HPP_
