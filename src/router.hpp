#pragma once
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/beast/http.hpp>

#include <map>
#include <functional>
#include <vector>
#include <string>

#include "helpers.hpp"
#include "context.hpp"

struct RouteKey {
  const method_t method;
  const std::string path;
};

bool operator<(const RouteKey& route1, const RouteKey& route2);

class Router {
public:
  Router(AppContext& context) : context(context) {};

  inline void register_handler(RouteKey key, request_handler_t handler) {
    simple_routes[key] = handler;
  }

  inline void register_mask_handler(RouteKey key, complex_request_handler_t handler) {
    complex_routes.push_back({ key, handler });
  }

  boost::asio::awaitable<response_t> route(const method_t method, const std::string& raw_path, request_t& request);
  bool compare_with_mask(RouteKey& route, RouteKey& mask, std::vector<std::string>& arguments);
private:
  AppContext& context;
  std::map<RouteKey, request_handler_t> simple_routes;
  std::vector<std::pair<RouteKey, complex_request_handler_t>> complex_routes;
};