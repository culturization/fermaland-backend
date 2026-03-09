file(GLOB SRC_FILES "*.cpp")
file(GLOB APP_FILES "app/**/*.cpp")

# add_subdirectory("app/")
add_executable(main ${SRC_FILES} ${APP_FILES})