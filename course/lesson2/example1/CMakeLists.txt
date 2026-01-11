cmake_minimum_required(VERSION 3.10)
project(simple_dashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Find nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)

# Simple Service
add_executable(simple_service simple_service.cpp)
target_link_libraries(simple_service 
    vsomeip3 
    ${Boost_LIBRARIES} 
    nlohmann_json::nlohmann_json
    pthread
)

# Simple Client
add_executable(simple_client simple_client.cpp)
target_link_libraries(simple_client 
    vsomeip3 
    ${Boost_LIBRARIES}
    pthread
)

# Copy data files
configure_file(vehicle_data.json ${CMAKE_BINARY_DIR}/vehicle_data.json COPYONLY)
