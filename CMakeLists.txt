set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_COMPILER "/msys64/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "/msys64/mingw64/bin/g++.exe")
cmake_minimum_required(VERSION 4.2)
project("fermaland")

include_directories("lib/boost")
include_directories("lib/rapidjson/include")
include_directories("lib/SQLiteCpp/include")
include_directories("lib/cryptopp")
include_directories("src/")
add_subdirectory("lib/SQLiteCpp")

set(Boost_ROOT "C:/Boost")
find_package(Boost REQUIRED)
set(NETWORK_LIBS Boost::headers ws2_32 Mswsock)
set(SQL_LIBS SQLiteCpp sqlite3)
set(OTHER_LIBS ${CMAKE_SOURCE_DIR}/lib/cryptopp/libcryptopp.a)

add_executable(main src/main.cpp)
target_link_libraries(main ${NETWORK_LIBS} ${SQL_LIBS} ${OTHER_LIBS})

add_executable(schema db/schema.cpp)
target_link_libraries(schema ${SQL_LIBS})