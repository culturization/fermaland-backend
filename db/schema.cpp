#include <SQLiteCpp/SQLiteCpp.h>

int main() {
  SQLite::Database db("sqlite.db", SQLite::OPEN_READWRITE);
  SQLite::Statement query(db, "CREATE TABLE IF NOT EXISTS users ("\
  "id INTEGER PRIMARY KEY,"\
  "name VARCHAR(64),"\
  "password_hash VARCHAR(64)"\
  ")");

  query.exec();
}