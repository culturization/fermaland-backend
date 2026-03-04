#include <boost/beast/http.hpp>
#include "router.cpp"
#include "base.cpp"
#include "app/app.hpp"
#include "context.hpp"

int main() {
  AppContext ctx;
  Router router;
  register_main_controller(router, ctx);

  HTTPBaseServer server(router);
  
  server.open();
}