/**
 * @dir main/ipc/
 *
 * @file Queue.hpp
 *
 * @brief Queue class definition.
 *
 * @author Timothy Logan <logantc@dukes.jmu.edu>
 */
#ifndef REDCROSS_LIB_IPC_QUEUE_HPP_
#define REDCROSS_LIB_IPC_QUEUE_HPP_

#include <mutex>
#include <list>
#include <condition_variable>

using namespace std;

/**
 * A thread-safe queue implementation.
 *
 * @tparam Tp the queue data type
 */
template<typename Tp>
class Queue {
 private:
  /**
   * The maximum size of the queue.
   */
  int maxSize;
  /**
   * The container this queue wraps around.
   */
  list<Tp> items;
  /**
   * This queue's mutex.
   */
  mutex mtx;
  /**
   * A condition variable for asynchronous operations on this queue.
   */
  condition_variable_any cond;
 public:
  /**
   * Creates an empty instance of this queue.
   */
  Queue()
	  : maxSize(-1),
		items(),
		mtx(),
		cond() {}
  /**
   * Creates an instance of this queue with a maximum capacity.
   *
   * @param maxSize the queue's capacity
   */
  explicit Queue(int maxSize)
	  : maxSize(maxSize),
		items(),
		mtx(),
		cond() {}
  /**
   * Whether this queue is empty.
   *
   * @return true if this queue is empty
   */
  bool empty() {
	return size() == 0;
  }

  /**
   * Returns the current size of this queue.
   *
   * @return the current size
   */
  int size() {
	lock_guard<mutex> lock(mtx);
	return static_cast<int>(items.size());
  }

  /**
   * Returns the queue capacity.
   *
   * @return the capacity
   */
  int length() const {
	return maxSize;
  }

  /**
   * Pushes a value reference onto this queue and notifies any waiters.
   *
   * @param value the value to insert
   * @return whether insertion succeeded
   */
  bool push(const Tp &value) {
	lock_guard<mutex> lock(mtx);
	if (items.size() < maxSize) {
	  items.emplace_back(value);
	  cond.notify_one();
	  return true;
	}
	return false;
  }

  /**
   * Pushes an rvalue reference onto this queue and notifies any waiters.
   *
   * @param value the value to insert
   * @return whether insertion succeeded
   */
  bool push(Tp &&value) {
	lock_guard<mutex> lck(mtx);
	if (items.size() < maxSize) {
	  items.emplace_back(move(value));
	  cond.notify_one();
	  return true;
	}
	return false;
  }

  /**
   * Pops an item from this queue and returns it through the target.
   *
   * @param target a reference to receive the popped value
   * @return whether the operation succeeded
   */
  bool pop(Tp &target) {
	lock_guard<mutex> lock(mtx);
	if (items.size > 0) {
	  target = items.front();
	  items.erase(items.begin());
	  return true;
	}
	return false;
  }

  /**
   * Waits for an indefinite amount of time for another thread to insert an
   * item into this queue.
   *
   * @param target a reference to receive the popped value
   */
  void wait_pop(Tp &target) {
	lock_guard<mutex> lock(mtx);
	while (items.empty())
	  cond.wait(lock);

	target = items.front();
	items.erase(items.begin());
  }

  /**
   * Pops an item if the predicate function succeeds determines it should.
   *
   * @param predicate the function to test
   * @return whether the operation succeeded
   */
  bool pop_if(function<bool(const Tp &front)> &&predicate) {
	lock_guard<mutex> lck(mtx);
	if (!items.empty() && predicate(items.front()))
	  items.erase(items.begin());
	else return false;
	return true;
  }

};

#endif // REDCROSS_LIB_IPC_QUEUE_HPP_
