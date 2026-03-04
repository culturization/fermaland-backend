set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_COMPILER "/msys64/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "/msys64/mingw64/bin/g++.exe")
cmake_minimum_required(VERSION 4.2)
project("fermaland")

include_directories("lib/rapidjson/include")
include_directories("lib/SQLiteCpp/include")
add_subdirectory("lib/SQLiteCpp")

set(Boost_ROOT "C:/Boost")
find_package(Boost REQUIRED)
set(NETWORK_LIBS Boost::headers ws2_32 Mswsock)
set(SQLITE_LIBS SQLiteCpp sqlite3)

add_executable(main src/main.cpp)
target_link_libraries(main ${NETWORK_LIBS} ${SQLITE_LIBS})

add_executable(schema db/schema.cpp)
target_link_libraries(schema ${SQLITE_LIBS})