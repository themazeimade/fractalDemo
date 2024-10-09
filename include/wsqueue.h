#pragma once
#include <deque>
#include <mutex>
#include <utilities.h>

class wsqueue {
private:
  typedef functionWrapper data_type;
  std::deque<data_type> thequeue;
  mutable std::mutex queueMutex;

public:
  wsqueue(){};
  wsqueue(const wsqueue &other) = delete;
  wsqueue &operator=(const wsqueue &other) = delete;
  void push(data_type O) {
    std::lock_guard<std::mutex> lock(queueMutex);
    thequeue.push_front(std::move(O));
  }
  bool empty() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return thequeue.empty();
  }
  bool try_pop(data_type &O) {
    //HEREE
    std::lock_guard<std::mutex> lock(queueMutex);
    if (thequeue.empty())
      return false;
    O = std::move(thequeue.front());
    thequeue.pop_front();
    return true;
  }
  bool try_steal(data_type &O) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (thequeue.empty())
      return false;
    O = std::move(thequeue.back());
    thequeue.pop_back();
    return true;
  }
};
