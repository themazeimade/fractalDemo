#include "tsqueue.h"
#include "wsqueue.h"
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <vector>

class stealpool {
  typedef functionWrapper task_type;
  std::atomic_bool done;
  ts_queue<task_type> pool_task_queue;
  std::vector<std::unique_ptr<wsqueue>> local_task_queues;
  inline thread_local static unsigned queue_index;
  inline thread_local static wsqueue *local_queue;
  std::vector<std::thread> threads;
  join_threads joiner;


  void worker_thread(unsigned index) {
    queue_index = index;
    local_queue = local_task_queues[queue_index].get();
    while (!done) {
      run_pending_task();
    }
  }

  bool popTask_fromLocal(task_type &task) {
    return local_queue && local_queue->try_pop(task);
  }
  bool popTask_fromPool(task_type &task) {
    return pool_task_queue.try_pop(task);
  }
  bool stealTask_fromOther(task_type &task) {
    for (unsigned i = 0; i < local_task_queues.size(); ++i) {
      unsigned const index = (queue_index + i + 1) % local_task_queues.size();
      if (local_task_queues[index]->try_steal(task)) {
        return true;
      }
    }
    return false;
  }

public:
  stealpool() : done(false), joiner(threads) {
    unsigned const thread_count = std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; ++i) {
        local_task_queues.push_back(std::unique_ptr<wsqueue>(new wsqueue));
      }
      for (unsigned i = 0; i < thread_count; ++i) {
        threads.push_back(std::thread(&stealpool::worker_thread, this, i));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }
  template <typename functiontype>
  std::future<typename std::result_of_t<functiontype()>::type>
  submit(functiontype f) {
    typedef typename std::result_of<functiontype()>::type result_type;
    std::packaged_task<result_type> task(f);
    std::future<result_type> result(task.get_future());
    if (local_queue) {
      local_queue->push(task);
    } else {
      pool_task_queue.push(task);
    }
    return result;
  }

  ~stealpool() { done = true; }

  void run_pending_task() {
    task_type task;
    if (popTask_fromLocal(task) || popTask_fromPool(task) ||
        stealTask_fromOther(task)) {
      task();
    } else {
      std::this_thread::yield();
    }
  }
};
