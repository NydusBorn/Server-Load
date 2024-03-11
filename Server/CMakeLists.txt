cmake_minimum_required(VERSION 3.27)
project(Server_Load CXX)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PostgreSQL REQUIRED)

add_executable(Server_Load main.cpp)

target_link_libraries(Server_Load PRIVATE PostgreSQL::PostgreSQL)
