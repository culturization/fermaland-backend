#pragma once
#include <thread>
#include <vector>
#include <map>

#include <boost/asio/io_context.hpp>

// ОЧЕНЬ ВАЖНАЯ ПЕРЕМЕННАЯ
thread_local int thread_num;

class ThreadsManager {
public:
  ThreadsManager() : threads_size(std::thread::hardware_concurrency()) {
    threads.resize(threads_size);
  }

  void start(boost::asio::io_context& io) {
    for (int i = 0; i < threads_size; i++) {
      std::thread thread([&io](int thread_num_) {
        thread_num = thread_num_;
        io.run();
      }, i);

      thread_nums[thread.get_id()] = i;
      threads.push_back(thread);
    }
    for (int i = 0; i < threads_size; i++) {
      threads[i].join();
    }
  }

  unsigned int threads_size;
  std::vector<std::thread> threads;
  std::map<std::thread::id, unsigned int> thread_nums;
};