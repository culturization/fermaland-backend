#include <boost/beast/http.hpp>
#include "router.hpp"
#include "base.hpp"
#include "app/app.hpp"
#include "context.hpp"
#include "threads_manager.hpp"

int main() {
  Router router;
  HTTPBaseServer server(router);
  ThreadsManager tm;
  AppContext ctx;

  register_main_controller(router, ctx);

  AuthService auth_service(tm.threads.size());
  register_auth_controller(router, ctx, auth_service);

  server.open(tm);
}