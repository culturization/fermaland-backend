#include "threads_manager.hpp"

thread_local int thread_num;

ThreadsManager::ThreadsManager() : threads(std::thread::hardware_concurrency()) {};

void ThreadsManager::start(boost::asio::io_context& io) {
  for (int i = 0; i < threads.size(); i++) {
    threads[i] = std::thread([&io](int thread_num_) {
      thread_num = thread_num_;
      io.run();
    }, i);
  }
  for (auto& thread : threads) thread.join();
}