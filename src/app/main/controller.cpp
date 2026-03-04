#include "controller.hpp"

void register_main_controller(Router& router) {
  using namespace boost::beast::http;

  ENDPOINT(hello) {
    co_return send_json(status::ok, "123", "456");
  };
  REGISTER_ENDPOINT(verb::get, "/hello", hello);
}