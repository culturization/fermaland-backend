#include "controller.hpp"

namespace MainController {
  ENDPOINT(hello) {
    co_return send_json(
      boost::beast::http::status::ok,
      "123", "456" // TODO: поменять структуру
    );
  }
}