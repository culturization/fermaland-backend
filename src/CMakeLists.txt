set(FILES "main.cpp" "base.cpp" "router.cpp" "helpers.cpp" "uri.cpp")
file(GLOB APP_FILES "app/**/*.cpp")
add_executable(main ${FILES} ${APP_FILES})