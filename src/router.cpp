#include "router.hpp"

bool operator<(const RouteKey& route1, const RouteKey& route2) { return route1.path < route2.path; }

Router::Router() {};

boost::asio::awaitable<response_t> Router::route(const method_t method, const std::string& raw_path, request_t& request) {
  int pos = 0;
  std::string path;

  if (raw_path == "/") {
    path = raw_path;
  } else {
    for (int i = 0; i < raw_path.length(); i++) {
      if (raw_path[i] == '?') break; // ignore query for now
      if (raw_path[i] != '/') pos = i;
    }
    path = raw_path.substr(0, pos + 1);
  }

  RouteKey route { method, path };

  if (auto it = simple_routes.find(route); it != simple_routes.end()) {
    auto func = it->second;
    co_return co_await func(request);
  } else {
    std::vector<std::string> arguments;

    bool found = false;
    complex_request_handler_t handler;

    for (auto& complex_route : complex_routes) {
      if (compare_with_mask(route, complex_route.first, arguments)) {
        found = true;
        handler = complex_route.second;
        break;
      } else {
        arguments.clear();
      }
    }

    if (found) {
      co_return co_await handler(request, std::move(arguments));
    } else {
      co_return return_http_error(boost::beast::http::status::not_found, "Not found");
    }
  };
}

bool Router::compare_with_mask(RouteKey& route, RouteKey& mask, std::vector<std::string>& arguments) {
  if (mask.method != route.method) return false;

  std::string accumulator;
  bool equal = true;
  int route_i = 0, mask_i = 0;

  while (true) {
    if (mask.path[mask_i] == '*') {
      if (route.path[route_i] != '/') {
        accumulator += route.path[route_i];
      } else {
        arguments.push_back(accumulator);
        accumulator.clear();
        mask_i += 2;
      }
    } else {
      if (mask.path[mask_i] != route.path[route_i]) { equal = false; break; }
      mask_i++;
    }
    route_i++;

    if (route_i >= route.path.length() || mask_i >= mask.path.length()) {
      equal = route_i == route.path.length() && mask_i == mask.path.length();
      break;
    }
  }
  if (accumulator.length() != 0) arguments.push_back(accumulator);
  
  return equal;
}