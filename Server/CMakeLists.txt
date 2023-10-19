cmake_minimum_required(VERSION 3.27)
project(Server_Load C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra -Wpedantic)

find_package(PostgreSQL REQUIRED)

add_executable(Server_Load main.c)

target_link_libraries(Server_Load PRIVATE PostgreSQL::PostgreSQL)
