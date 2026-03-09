#pragma once
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"

#include <iostream>
#include <optional>
#include <format>

#include "helpers.hpp"

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
response_t send_json(const boost::beast::http::status status_code, Args... args);

response_t return_http_error(const boost::beast::http::status status_code, const char* message);

inline response_t bad_request(const char* message) {
  return return_http_error(boost::beast::http::status::bad_request, message);
}

inline response_t bad_request() {
  return bad_request("Bad request");
}

enum class JSON_t {
  String,
  Integer,
  Double,
  Bool,
  Object,
  Array
};
typedef std::tuple<const char*, JSON_t> ValidatorDefaultArg;
typedef std::tuple<const char*, JSON_t, size_t, size_t> ValidatorRangeArg;
typedef std::variant<ValidatorDefaultArg, ValidatorRangeArg> ValidatorArg;

std::optional<response_t> validate_impl(rapidjson::Document& json, const ValidatorDefaultArg arg);
std::optional<response_t> validate_impl(rapidjson::Document& json, const ValidatorRangeArg arg);

template <typename... Args>
inline std::optional<response_t> validate_impl(rapidjson::Document& json, const ValidatorArg arg, Args... args) {
  std::optional<response_t> response;
  if (std::holds_alternative<ValidatorDefaultArg>(arg)) {
    response = validate_impl(json, std::get<ValidatorDefaultArg>(arg));
  } else {
    response = validate_impl(json, std::get<ValidatorRangeArg>(arg));
  }
  if (response.has_value()) return response;
  return validate_impl(json, args...);
}

template <typename... Args>
inline std::optional<response_t> validate(rapidjson::Document& json, Args... args) {
  if (!json.IsObject()) return bad_request("Expected the root value to be an object");
  return validate_impl(json, args...);
}

template <typename... Args>
inline std::optional<response_t> json_parse(rapidjson::Document& json, const request_t& request, Args... args) {
  if (json.Parse(request.body().data()).HasParseError()) return bad_request();
  return validate(json, args...);
}