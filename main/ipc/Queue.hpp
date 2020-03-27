/**
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

template<typename Tp>
class Queue {
 private:
  int maxSize;
  list<Tp> items;
  mutex mtx;
  condition_variable_any cond;
 public:
  Queue()
	  : maxSize(-1),
		items(),
		mtx(),
		cond() {}
  explicit Queue(int maxSize)
	  : maxSize(maxSize),
		items(),
		mtx(),
		cond() {}

  bool empty() {
	return size() == 0;
  }

  int size() {
	lock_guard<mutex> lock(mtx);
	return static_cast<int>(items.size());
  }

  int length() const {
	return maxSize;
  }

  bool push(const Tp &value) {
	lock_guard<mutex> lock(mtx);
	if (items.size() < maxSize) {
	  items.emplace_back(value);
	  cond.notify_one();
	  return true;
	}
	return false;
  }

  bool push(Tp &&value) {
	lock_guard<mutex> lck(mtx);
	if (items.size() < maxSize) {
	  items.emplace_back(move(value));
	  cond.notify_one();
	  return true;
	}
	return false;
  }

  bool pop(Tp &target) {
	lock_guard<mutex> lock(mtx);
	if (items.size > 0) {
	  target = items.front();
	  items.erase(items.begin());
	  return true;
	}
	return false;
  }

  void wait_pop(Tp &target) {
	lock_guard<mutex> lock(mtx);
	while (items.empty())
	  cond.wait(lock);

	target = items.front();
	items.erase(items.begin());
  }

  bool pop_if(function<bool(const Tp &front)> &&predicate) {
	lock_guard<mutex> lck(mtx);
	if (!items.empty() && predicate(items.front()))
	  items.erase(items.begin());
	else return false;
	return true;
  }

};

#endif // REDCROSS_LIB_IPC_QUEUE_HPP_
