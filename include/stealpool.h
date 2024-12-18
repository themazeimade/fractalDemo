#pragma once
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
  const unsigned int thread_count;
  ts_queue<task_type> pool_task_queue;
  std::vector<std::unique_ptr<wsqueue>> local_task_queues;
  std::vector<std::thread> threads;
  join_threads joiner;
  inline static thread_local wsqueue *local_queue;
  inline static thread_local unsigned queue_index;

  void worker_thread(unsigned index) {
    queue_index = index;
    local_queue = local_task_queues[queue_index].get();
    /*std::cout << queue_index << std::endl;*/

    while (!done) {
      run_pending_task();
    }
  }

  bool popTask_fromLocal(task_type &task) {
    bool res = local_queue && local_queue->try_pop(task);
    return res;
  }
  bool popTask_fromPool(task_type &task) {
    /*std::cout << "poptask from pool" << std::endl;*/
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
  stealpool() : done(false), thread_count(std::thread::hardware_concurrency()-4) , joiner(threads) {
    /*thread_count = std::thread::hardware_concurrency();*/
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

  bool isDone (){
    return pool_task_queue.getRemaining_tasks() == 0;
  }

  int getRamaining_task() {
    return pool_task_queue.getRemaining_tasks();
  }

  int TaskCount() { return pool_task_queue.getsize(); }

  int getThreadCount() {return thread_count;}

  template <typename functiontype>
  std::future<typename std::result_of<functiontype(void)>::type>
  submit(functiontype f) {
    typedef typename std::result_of<functiontype()>::type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> result(task.get_future());
    if (local_queue) {
      local_queue->push(std::move(task));
    } else {
      pool_task_queue.push(std::move(task));
    }
    return result;
  }

  ~stealpool() { done = true; }

  void run_pending_task() {
    task_type task;
    if (popTask_fromLocal(task) || popTask_fromPool(task) ||
        stealTask_fromOther(task)) {
      /*pool_task_queue.TaskOneUp();*/
      task();
      pool_task_queue.TaskDone();
    } else {
      std::this_thread::yield();
    }
  }
};

extern stealpool Tpool;
/*static constexpr unsigned stealpool::thread_count = std::thread::hardware_concurrency(); */
