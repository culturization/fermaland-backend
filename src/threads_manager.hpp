#pragma once
#include <thread>
#include <vector>
#include <map>

#include <boost/asio/io_context.hpp>

// ОЧЕНЬ ВАЖНАЯ ПЕРЕМЕННАЯ
thread_local int thread_num;

class ThreadsManager {
public:
  ThreadsManager() : threads(std::thread::hardware_concurrency()) {};

  void start(boost::asio::io_context& io) {
    for (int i = 0; i < threads.size(); i++) {
      threads[i] = std::thread([&io](int thread_num_) {
        thread_num = thread_num_;
        io.run();
      }, i);
    }
    for (auto& thread : threads) thread.join();
  }

  std::vector<std::thread> threads;
};