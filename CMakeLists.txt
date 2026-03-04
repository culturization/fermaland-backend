cmake_minimum_required(VERSION 4.2)
project("fermaland")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

include_directories("lib/rapidjson/include")
add_subdirectory("src")

set(Boost_ROOT "C:/Boost")
find_package(Boost REQUIRED)
target_link_libraries(main Boost::headers ws2_32 Mswsock)