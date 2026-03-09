#pragma once
#include <boost/asio/awaitable.hpp>
#include "rapidjson/document.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <sqlite3.h>

#include "json.hpp"
#include "../../router.hpp"
#include "../../context.hpp"
#include "service.hpp"

void register_auth_controller(Router& router, AppContext& ctx, AuthService& auth_service);