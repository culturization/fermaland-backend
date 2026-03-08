#include "base.hpp"

void HTTPBaseServer::open(ThreadsManager& tm) {
  asio::ip::address addr = asio::ip::make_address("0.0.0.0");
  const int port = 8000;

  asio::io_context io { tm.threads_size };
  asio::co_spawn(io, listen(), handle_exception_l);
  tm.start(io);
}

void HTTPBaseServer::handle_exception(std::exception_ptr e) {
  if(e) {
    try {
      std::rethrow_exception(e);
    } catch(const std::exception& e) {
      printf("err: %s\n", e.what());
    }
  }
}

asio::awaitable<void> HTTPBaseServer::listen() {
  auto executor = co_await asio::this_coro::executor;
  auto acceptor = asio::ip::tcp::acceptor { executor, endpoint };

  while (true) {
    asio::co_spawn(
      executor, handle_connection(beast::tcp_stream { co_await acceptor.async_accept() }),
      handle_exception_l
    );
  }
}

asio::awaitable<void> HTTPBaseServer::handle_connection(beast::tcp_stream tcp) {
  using namespace beast;

  flat_buffer buffer;
  bool keep_alive = true;

  while (keep_alive) {
    tcp.expires_after(std::chrono::seconds(60));

    request_t request;
    try {
      co_await http::async_read(tcp, buffer, request);
    } catch (const std::exception& e) {
      break;
    }
    keep_alive = request.keep_alive();

    http::message_generator response = co_await handle_request(std::move(request));
    co_await beast::async_write(tcp, std::move(response));
  }

  if (tcp.socket().is_open()) tcp.socket().shutdown(net::ip::tcp::socket::shutdown_send);
}

asio::awaitable<beast::http::message_generator> HTTPBaseServer::handle_request(request_t request) {
  using namespace beast;

  const method_t method = request.method();
  const std::string path = request.target();

  response_t response = co_await router.route(method, path, request);
  set_headers(response, request);

  co_return response;
}

void HTTPBaseServer::set_headers(response_t& response, request_t& request) {
  using namespace beast;

  response.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  response.set(http::field::content_type, "application/json");
  response.keep_alive(request.keep_alive());
  response.prepare_payload();
}