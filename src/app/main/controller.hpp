#pragma once
#include <boost/asio/awaitable.hpp>

#include "json.hpp"
#include "router.hpp"
#include "context.hpp"
#include "service.hpp"

void register_main_controller(Router& router, AppContext& ctx);