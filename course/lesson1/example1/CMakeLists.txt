cmake_minimum_required(VERSION 3.10)
project(hello_someip)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find vsomeip
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Hello Service
add_executable(hello_service hello_service.cpp)
target_link_libraries(hello_service 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Hello Client
add_executable(hello_client hello_client.cpp)
target_link_libraries(hello_client 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Copy config file
configure_file(vsomeip.json ${CMAKE_BINARY_DIR}/vsomeip.json COPYONLY)
