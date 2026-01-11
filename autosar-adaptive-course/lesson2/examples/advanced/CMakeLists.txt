cmake_minimum_required(VERSION 3.10)
project(dashboard_advanced)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Service
add_executable(dashboard_service_advanced 
    dashboard_service_advanced.cpp
)
target_link_libraries(dashboard_service_advanced 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Client
add_executable(dashboard_client_advanced 
    dashboard_client_advanced.cpp
)
target_link_libraries(dashboard_client_advanced 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Copy resources
configure_file(vehicle_database_full.json 
    ${CMAKE_BINARY_DIR}/vehicle_database_full.json COPYONLY)

# Install
install(TARGETS dashboard_service_advanced dashboard_client_advanced
    RUNTIME DESTINATION bin)
install(FILES vehicle_database_full.json DESTINATION etc)
