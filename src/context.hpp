#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

class AppContext {
public:
  AppContext() : db("sqlite.db", SQLite::OPEN_READWRITE) {};
  SQLite::Database db;
};