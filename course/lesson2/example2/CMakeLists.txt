cmake_minimum_required(VERSION 3.10)
project(dashboard_service)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Include ara:: headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Dashboard Service
add_executable(dashboard_service dashboard_service.cpp)
target_link_libraries(dashboard_service 
    vsomeip3 
    ${Boost_LIBRARIES} 
    nlohmann_json::nlohmann_json
    pthread
)

# Dashboard Client
add_executable(dashboard_client dashboard_client.cpp)
target_link_libraries(dashboard_client 
    vsomeip3 
    ${Boost_LIBRARIES}
    pthread
)

# Copy config files
configure_file(database.json ${CMAKE_BINARY_DIR}/database.json COPYONLY)
configure_file(vsomeip.json ${CMAKE_BINARY_DIR}/vsomeip.json COPYONLY)

# Installation
install(TARGETS dashboard_service dashboard_client DESTINATION bin)
install(FILES database.json vsomeip.json DESTINATION etc)
