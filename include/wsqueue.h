#include <deque>
#include <mutex>
#include <utilities.h>

class wsqueue {
private:
  typedef functionWrapper data_type;
  std::deque<data_type> thequeue;
  mutable std::mutex queueMutex;
public:
  wsqueue() = default;
  wsqueue(const wsqueue&) = delete;
  wsqueue& operator=(const wsqueue&) = delete;
  bool empty() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return thequeue.empty();
  }
  bool try_pop(data_type& O) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (empty()) return false;
    O = std::move(thequeue.front());
    thequeue.pop_front();
    return true;
  }
  bool push(data_type& O) {
    std::lock_guard<std::mutex> lock(queueMutex);
    thequeue.push_front(std::move(O));
    return true;
    
  }
  bool try_steal(data_type& O){
    std::lock_guard<std::mutex> lock(queueMutex);
    if (empty()) return false;
    O = std::move(thequeue.back());
    thequeue.pop_back();
    return true;
  }

};
