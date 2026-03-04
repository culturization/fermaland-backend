#pragma once
#include <boost/asio/awaitable.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include <cstdint>
#include <iostream>
#include "context.hpp"

#define ENDPOINT [&ctx](request_t& request) -> boost::asio::awaitable<response_t>
#define MASK_ENDPOINT [&ctx](request_t& request, std::vector<std::string> query) -> boost::asio::awaitable<response_t>

using response_t = boost::beast::http::response<boost::beast::http::string_body>;
using request_t = boost::beast::http::request<boost::beast::http::string_body>;
using method_t = boost::beast::http::verb;
using request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&)>;
using complex_request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&, std::vector<std::string> query)>;

response_t return_http_error(boost::beast::http::status status_code, const char* message);

template <typename T>
inline void json_add_members(rapidjson::Document& json, const char* key, T&& value) {
  json_add_members(json, key, rapidjson::Value(value));
}

inline void json_add_members(rapidjson::Document& json, const char* key, rapidjson::Value value) {
  json.AddMember(rapidjson::Value(key, json.GetAllocator()), value, json.GetAllocator());
}

inline void json_add_members(rapidjson::Document& json, const char* key, const char* value) {
  json_add_members(json, key, rapidjson::Value(value, json.GetAllocator())); // TODO: maybe optimize later
}

template <typename T, typename... Args>
inline void json_add_members(rapidjson::Document& json, const char* key, T&& value, Args... args) {
  json_add_members(json, key, value);
  json_add_members(json, args...);
}

template <typename... Args>
rapidjson::Document generate_json(Args... args) {
  rapidjson::Document json;
  json.SetObject();
  json_add_members(json, args...);

  return json;
}

template <typename... Args>
response_t send_json(boost::beast::http::status status_code, Args... args) {
  rapidjson::Document json = generate_json(args...);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  json.Accept(writer);

  response_t response { status_code, 11 };
  response.body() = buffer.GetString();

  return response;
}

response_t return_http_error(boost::beast::http::status status_code, const char* message) {
  return send_json(
    status_code,
    "error", message
  );
}
