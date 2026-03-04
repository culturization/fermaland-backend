#include "helpers.hpp"

response_t return_http_error(boost::beast::http::status status_code, const char* message) {
  return send_json(
    status_code,
    "error", message
  );
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