#include "helpers.hpp"

response_t send_body(const boost::beast::http::status status_code, const char* body) {
  response_t response { status_code, 11 };
  response.body() = body;
  return response;
}