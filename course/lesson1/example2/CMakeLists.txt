cmake_minimum_required(VERSION 3.10)
project(advanced_someip)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Advanced Service
add_executable(advanced_service advanced_service.cpp)
target_link_libraries(advanced_service 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Advanced Client
add_executable(advanced_client advanced_client.cpp)
target_link_libraries(advanced_client 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Copy config files
configure_file(vsomeip_advanced.json ${CMAKE_BINARY_DIR}/vsomeip.json COPYONLY)
