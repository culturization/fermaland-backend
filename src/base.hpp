#pragma once
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include <thread>
#include <string>
#include <vector>
#include <cstdio>

#include "router.hpp"

using namespace boost;

class HTTPBaseServer {
public:
  HTTPBaseServer(Router& router) : endpoint(asio::ip::make_address("0.0.0.0"), 8000), router(router) {}

  void open();
  void handle_exception(std::exception_ptr e);
  std::function<void(std::exception_ptr e)> handle_exception_l { [this](std::exception_ptr e) { handle_exception(e); } };

  asio::awaitable<void> listen();
  asio::awaitable<void> handle_connection(beast::tcp_stream tcp);
  asio::awaitable<beast::http::message_generator> handle_request(request_t request);
  void set_headers(response_t& response, request_t& request);
private:
  asio::ip::tcp::endpoint endpoint;
  Router& router;
};