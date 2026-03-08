#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

class AppContext {
public:
  AppContext(ThreadsManager& tm) : db("sqlite.db", SQLite::OPEN_READWRITE), tm(tm) {};
  SQLite::Database db;
  ThreadsManager& tm;
};