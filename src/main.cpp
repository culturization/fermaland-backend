#include <boost/beast/http.hpp>
#include "router.hpp"
#include "base.hpp"
#include "app/main/controller.hpp"

int main() {
  Router router;
  router.register_handler({ boost::beast::http::verb::get, "/hello" }, MainController::hello);

  HTTPBaseServer server(router);
  
  server.open();
}