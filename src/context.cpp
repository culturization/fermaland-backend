#include "context.hpp"

AppContext::AppContext() : db("sqlite.db", SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE) {};