cmake_minimum_required(VERSION 3.10)
project(QtSqlExample)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Core Sql REQUIRED)

add_executable(QtSqlExample example_usage.cpp)
target_link_libraries(QtSqlExample Private Qt6::Core Qt6::Sql)
