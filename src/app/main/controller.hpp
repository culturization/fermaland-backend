#pragma once
#include <boost/asio/awaitable.hpp>
#include "../../helpers.hpp"

namespace MainController {
  /* GET /hello
   * Returns nothing.
   */
  ENDPOINT(hello);
}