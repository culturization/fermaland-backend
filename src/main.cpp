#include <boost/beast/http.hpp>
#include "router.cpp"
#include "base.cpp"
#include "app/app.hpp"
#include "context.hpp"
#include "threads_manager.hpp"

int main() {
  Router router;
  HTTPBaseServer server(router);
  ThreadsManager tm;
  AppContext ctx(tm);

  register_main_controller(router, ctx);

  AuthService auth_service;
  register_auth_controller(router, ctx, auth_service);

  server.open(tm);
}