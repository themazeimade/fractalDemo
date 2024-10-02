#pragma once
#include <memory>
#include <thread>
#include <vector>

class functionWrapper {
  struct impl_b {
    virtual void call() {};
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
