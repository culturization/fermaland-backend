#pragma once
#include <boost/asio/awaitable.hpp>
#include "helpers.hpp"
#include "router.hpp"
#include "service.hpp"

void register_auth_controller(Router& router, AppContext& ctx, AuthService& auth_service) {
  using namespace boost::beast::http;

  router.register_handler({ verb::post, "/auth/register" }, ENDPOINT {

  });

  router.register_handler({ verb::post, "/auth/login" }, ENDPOINT {

  });
}