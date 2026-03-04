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

#define ENDPOINT(name) auto name = [](request_t& request, AppContext& ctx) -> boost::asio::awaitable<response_t> 
#define REGISTER_ENDPOINT(method, path, name) router.register_handler({ method, path }, name);
#define MASK_ENDPOINT(name) auto name = [](request_t& request, AppContext& ctx, std::vector<std::string>) -> boost::asio::awaitable<response_t> 
#define REGISTER_MASK_ENDPOINT(method, path, name) router.register_mask_handler({ method, path }, name);

using response_t = boost::beast::http::response<boost::beast::http::string_body>;
using request_t = boost::beast::http::request<boost::beast::http::string_body>;
using method_t = boost::beast::http::verb;
using request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&, AppContext& ctx)>;
using complex_request_handler_t = std::function<boost::asio::awaitable<response_t>(request_t&, AppContext& ctx, std::vector<std::string> args)>;

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
rapidjson::Document generate_json(Args... args);

template <typename... Args>
response_t send_json(boost::beast::http::status status_code, Args... args);