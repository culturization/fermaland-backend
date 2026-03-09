#include <SQLiteCpp/SQLiteCpp.h>

int main() {
  SQLite::Database db("sqlite.db", SQLite::OPEN_READWRITE);
  SQLite::Transaction t(db);

  db.exec("CREATE TABLE IF NOT EXISTS users("
  "id INTEGER PRIMARY KEY,"
  "name VARCHAR(64) NOT NULL,"
  "email VARCHAR(64) UNIQUE NOT NULL,"
  "password_hash VARCHAR(64) NOT NULL"
  ")");

  db.exec("CREATE TABLE IF NOT EXISTS tokens_users("
  "token VARCHAR(32) UNIQUE NOT NULL,"
  "user_id INTEGER NOT NULL,"
  "FOREIGN KEY (user_id) REFERENCES users(id)"
  ")");
  db.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_token ON tokens_users(token)");

  t.commit();
}