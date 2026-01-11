cmake_minimum_required(VERSION 3.10)
project(dashboard_basic)

set(CMAKE_CXX_STANDARD 14)

find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

add_executable(dashboard_service_basic dashboard_service_basic.cpp)
target_link_libraries(dashboard_service_basic vsomeip3 ${Boost_LIBRARIES} pthread)

add_executable(dashboard_client_basic dashboard_client_basic.cpp)
target_link_libraries(dashboard_client_basic vsomeip3 ${Boost_LIBRARIES} pthread)

# Copy JSON database
configure_file(vehicle_database.json ${CMAKE_BINARY_DIR}/vehicle_database.json COPYONLY)
