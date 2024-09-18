/*#pragma once*/

#include <future>
#include <mutex>
#include <thread>
#include <vector>

class functionWrapper {
  struct impl_b {
    virtual void call(){};
    virtual ~impl_b(){};
  };
  std::unique_ptr<impl_b> impl;
  template <typename F> struct impl_type : impl_b {
    F f;
    impl_type(F &&f_) : f(std::move(f_)){};
    void call() { f(); };
    ~impl_type() {}
  };

public:
  template <typename F>
  functionWrapper(F &&f) : impl(new impl_type<F>(std::move(f))) {}
  void operator()() { impl->call(); }
  functionWrapper() = default;
  functionWrapper(functionWrapper &&other) : impl(std::move(other.impl)) {}
  functionWrapper &operator=(functionWrapper &&other) {
    impl = std::move(other.impl);
    return *this;
  }
  functionWrapper(const functionWrapper &) = delete;
  functionWrapper(functionWrapper &) = delete;
  functionWrapper &operator=(const functionWrapper &) = delete;
};

class join_threads {
  std::vector<std::thread> &threads;

public:
  explicit join_threads(std::vector<std::thread> &threads_)
      : threads(threads_) {}
  ~join_threads() {
    for (unsigned long i = 0; i < threads.size(); ++i) {
      if (threads[i].joinable())
        threads[i].join();
    }
  }
};

template <typename T> class ts_queue {
private:
  struct node {
    std::shared_ptr<T> data;
    std::unique_ptr<node> next;
  };
  std::mutex head_mutex;
  std::unique_ptr<node> head;
  std::mutex tail_mutex;
  node *tail;
  std::condition_variable data_cond;

  node *get_tail();
  std::unique_ptr<node> pop_head();
  std::unique_lock<std::mutex> wait_for_data();
  std::unique_ptr<node> wait_pop_head();
  std::unique_ptr<node> wait_pop_head(T &value);
  std::unique_ptr<node> try_pop_head();
  std::unique_ptr<node> try_pop_head(T &value);

public:
  ts_queue() : head(new node), tail(head.get()) {}
  ts_queue(const ts_queue &other) = delete;
  ts_queue &operator=(const ts_queue &other) = delete;

  std::shared_ptr<T> try_pop();
  bool try_pop(T &value);
  std::shared_ptr<T> wait_and_pop();
  void wait_and_pop(T &value);
  void push(T new_value);
  bool empty();
};

class threadpool {
  std::atomic_bool done;
  ts_queue<functionWrapper> work_queue;
  std::vector<std::thread> threads;
  join_threads joiner;
  void worker_thread() {
    while (!done) {
      functionWrapper task;
      if (work_queue.try_pop(task)) {
        task();
      } else {
        std::this_thread::yield();
      }
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
  ~threadpool() { done = true; }
  template <typename FunctionType>
  std::future<typename std::result_of<FunctionType(void)>::type>
  submit(FunctionType f) {
    typedef typename std::result_of<FunctionType()>::type result_type;
    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res(task.get_future());
    work_queue.push(std::move(task));
    return res;
  }
};

template <typename T> void ts_queue<T>::push(T new_value) {
  std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
  std::unique_ptr<node> p(new node);
  {
    std::lock_guard<std::mutex> tail_lock(tail_mutex);
    tail->data = new_data;
    node *const new_tail = p.get();
    tail->next = std::move(p);
    tail = new_tail;
  }
  data_cond.notify_one();
}

template <typename T> typename ts_queue<T>::node *ts_queue<T>::get_tail() {
  std::lock_guard<std::mutex> tail_lock(tail_mutex);
  return tail;
}

template <typename T>
std::unique_ptr<typename ts_queue<T>::node> ts_queue<T>::pop_head() {
  std::unique_ptr<node> old_head = std::move(head);
  head = std::move(old_head->next);
  return old_head;
}

template <typename T>
std::unique_lock<std::mutex> ts_queue<T>::wait_for_data() {
  std::unique_lock<std::mutex> head_lock(head_mutex);
  data_cond.wait(head_lock, [&] { return head.get() != get_tail(); });
  return std::move(head_lock);
}

template <typename T>
std::unique_ptr<typename ts_queue<T>::node> ts_queue<T>::wait_pop_head() {
  std::unique_lock<std::mutex> head_lock(wait_for_data());
  return pop_head();
}

template <typename T>
std::unique_ptr<typename ts_queue<T>::node>
ts_queue<T>::wait_pop_head(T &value) {
  std::unique_lock<std::mutex> head_lock(wait_for_data());
  value = std::move(*head->data);
  return pop_head();
}

template <typename T>
std::unique_ptr<typename ts_queue<T>::node> ts_queue<T>::try_pop_head() {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  if (head.get() == get_tail()) {
    return std::unique_ptr<node>();
  }
  return pop_head();
}

template <typename T>
std::unique_ptr<typename ts_queue<T>::node>
ts_queue<T>::try_pop_head(T &value) {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  if (head.get() == get_tail()) {
    return std::unique_ptr<node>();
  }
  value = std::move(*head->data);
  return pop_head();
}

template <typename T> std::shared_ptr<T> ts_queue<T>::try_pop() {
  std::unique_ptr<node> old_head = try_pop_head();
  return old_head ? old_head->data : std::shared_ptr<T>();
}

template <typename T> bool ts_queue<T>::try_pop(T &value) {
  std::unique_ptr<node> const old_head = try_pop_head(value);
  return old_head ? true : false;
}

template <typename T> bool ts_queue<T>::empty() {
  std::lock_guard<std::mutex> head_lock(head_mutex);
  return (head.get() == get_tail());
}
