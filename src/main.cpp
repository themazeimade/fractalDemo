#include "threadpool.h"
#include <chrono>
#include <iostream>

bool job() {
  std::this_thread::sleep_for(std::chrono::milliseconds(700));
  std::cout << "task completed by thread: " << std::this_thread::get_id()
            << std::endl;
  return true;
}

int main() {
  threadpool pool;
  /*std::future<bool> i = pool.submit([]{return job();});*/
  /*std::cout << i.get() << std::endl;*/
  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; i++) {
    pool.submit([] { job(); });
  }
  auto end = std::chrono::high_resolution_clock::now();

  auto start1 = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; i++) {
    job();
  }
  auto end1 = std::chrono::high_resolution_clock::now();

  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  auto duration1 =
      std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);

  std::cout << "Time taken with thread: " << duration.count() << "microseconds"
            << std::endl;
  std::cout << "Time taken without thread: " << duration1.count()
            << "microseconds" << std::endl;

  /*bool done = false;*/
  /*while (!done) {*/
  /*  int a;*/
  /*  std::cin >> a;*/
  /*  if (a>0) {*/
  /*    pool.submit([]{job();});*/
  /*  } else return 0;*/
  /*}*/
  return 0;
}
