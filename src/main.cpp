#include <boost/beast/http.hpp>
#include "router.hpp"
#include "base.hpp"
#include "app/main/controller.hpp"
#include "context.hpp"

int main() {
  AppContext ctx;

  Router router(ctx);
  register_main_controller(router);

  HTTPBaseServer server(router);
  
  server.open();
}