#pragma once

#include "tsqueue.h"
#include "utilities.h"
#include <future>
#include <queue>
#include <thread>
#include <vector>

class threadpool { // with steal implementation
  std::atomic_bool done;
  ts_queue<functionWrapper> pool_work_queue;
  typedef std::queue<functionWrapper> local_queue_type;
  inline static thread_local std::unique_ptr<local_queue_type> local_work_queue;
  std::vector<std::thread> threads;
  join_threads joiner;
  void worker_thread() {
    local_work_queue.reset(new local_queue_type);
    while (!done) {
      runPendingTask();
    }
  }

public:
  threadpool() : done(false), joiner(threads) {
    unsigned const thread_count = std::thread::hardware_concurrency();
    try {
      for (unsigned i = 0; i < thread_count; i++) {
        threads.push_back(std::thread(&threadpool::worker_thread, this));
      }
    } catch (...) {
      done = true;
      throw;
    }
  }
  ~threadpool() {
    done = true;
  }
  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType(void)>::type>
  submit(FunctionType f) { // function
    typedef typename std::result_of<FunctionType()>::type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    if (local_work_queue) {
      local_work_queue->push(std::move(task));
    } else {
      pool_work_queue.push(std::move(task));
    }
    return res;
  }
  void runPendingTask() {
    functionWrapper task;
    if (local_work_queue && !local_work_queue->empty()) {
      task = std::move(local_work_queue->front());
      local_work_queue->pop();
      task();
    } else if (pool_work_queue.try_pop(task)) {
      task();
    } else {
      std::this_thread::yield();
    }
  }
};
