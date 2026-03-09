#include "controller.hpp"

void register_auth_controller(Router& router, AppContext& ctx, AuthService& auth_service) {
  using namespace boost::beast::http;
  using namespace rapidjson;

  router.register_handler({ verb::post, "/auth/register" }, [&ctx, &auth_service](request_t& request) -> boost::asio::awaitable<response_t> {
    Document json;
    std::optional<response_t> validator_res = json_parse(json, request,
      std::make_tuple("name",  JSON_t::String, 3, 64),
      std::make_tuple("email", JSON_t::String, 5, 64),
      std::make_tuple("pass",  JSON_t::String, 8, 256)
    );
    if (validator_res.has_value()) co_return validator_res.value();

    CryptoPP::byte salt[SALT_SIZE];
    auth_service.generate_salt(salt);
    const std::string password_hash = auth_service.generate_password_hash(json["pass"].GetString(), salt) ;

    SQLite::Statement query(ctx.db,
    "INSERT INTO users(name, email, password_hash)"
    "VALUES(?, ?, ?)"
    );
    query.bind(1, json["name"].GetString());
    query.bind(2, json["email"].GetString());
    query.bind(3, password_hash);

    try {
      query.exec();
    } catch (SQLite::Exception& e) {
      if (e.getErrorCode() == SQLITE_CONSTRAINT) {
        co_return return_http_error(status::forbidden, "Account with such an email already exists");
      } else {
        co_return return_http_error(status::internal_server_error, "Unable to create a new account");
      }
    }

    co_return send_body(status::ok, "{\"ok\":true}");
  });

  router.register_handler({ verb::post, "/auth/login" }, ENDPOINT {

  });
}