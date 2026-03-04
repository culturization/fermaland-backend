#pragma once
#include <boost/asio/awaitable.hpp>
#include "../../helpers.hpp"
#include "../../router.hpp"
/** @file main/controller.hpp
 * The main controller.
 */

void register_main_controller(Router& router, AppContext& ctx) {
  using namespace boost::beast::http;

  /** ### GET /hello
   * @brief Description
   * 
   * Response
   */
  router.register_handler({ verb::get, "/hello" }, ENDPOINT {
    co_return send_json(status::ok, "123", "456");
  });
}