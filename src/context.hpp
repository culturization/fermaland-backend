#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

class AppContext {
public:
  AppContext();
  SQLite::Database db;
};