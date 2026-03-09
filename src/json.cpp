#include "json.hpp"

template <typename... Args>
rapidjson::Document generate_json(Args... args) {
  rapidjson::Document json;
  json.SetObject();
  json_add_members(json, args...);

  return json;
}

template <typename... Args>
response_t send_json(const boost::beast::http::status status_code, Args... args) {
  rapidjson::Document json = generate_json(args...);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  json.Accept(writer);

  return send_body(status_code, buffer.GetString());
}

response_t return_http_error(const boost::beast::http::status status_code, const char* message) {
  return send_json(
    status_code,
    "error", message
  );
}

std::optional<response_t> validate_impl(rapidjson::Document& json, const ValidatorDefaultArg arg) {
  auto& [key, type] = arg;
  if (!json.HasMember(key)) return bad_request(std::format("Expected key \"{}\"", key).c_str());

  #define VALIDATE(type, method) case JSON_t::type:\
    if (!json[key].method()) {\
      return bad_request(std::format("Expected key \"{}\" of type" #type, key).c_str());\
    }\
    break;

  switch (type) {
  VALIDATE(String, IsString)
  VALIDATE(Integer, IsInt)
  VALIDATE(Double, IsDouble)
  VALIDATE(Bool, IsBool)
  VALIDATE(Object, IsObject)
  VALIDATE(Array, IsArray)
  }

  #undef VALIDATE
  return std::nullopt;
}

std::optional<response_t> validate_impl(rapidjson::Document& json, const ValidatorRangeArg arg) {
  const auto& [key, type, min, max] = arg;
  if (!json.HasMember(key)) return bad_request(std::format("Expected key \"{}\"", key).c_str());
  // assert(type == JSON_t::String)

  auto& val = json[key];
  if (!val.IsString()) return bad_request(std::format("Expected key \"{}\" of type String", key).c_str());

  rapidjson::SizeType strsize = val.GetStringLength();
  if (min > strsize) return bad_request(std::format("Key \"{}\" is too short", key).c_str());
  if (strsize > max) return bad_request(std::format("Key \"{}\" is too long", key).c_str());

  return std::nullopt;
}