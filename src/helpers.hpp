#pragma once
#include <boost/asio/awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#define ENDPOINT [&ctx](request_t& request) -> boost::asio::awaitable<response_t>
#define MASK_ENDPOINT [&ctx](request_t& request, std::vector<std::string> query) -> boost::asio::awaitable<response_t>

using response_t = boost::beast::http::response<boost::beast::http::string_body>;
using request_t = boost::beast::http::request<boost::beast::http::string_body>;
using method_t = boost::beast::http::verb;
using request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&)>;
using complex_request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&, std::vector<std::string> query)>;

response_t send_body(const boost::beast::http::status status_code, const char* body);