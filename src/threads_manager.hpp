#pragma once
#include <thread>
#include <vector>

#include <boost/asio/io_context.hpp>

// ОЧЕНЬ ВАЖНАЯ ПЕРЕМЕННАЯ
extern thread_local int thread_num;

class ThreadsManager {
public:
  ThreadsManager();
  void start(boost::asio::io_context& io);

  std::vector<std::thread> threads;
};