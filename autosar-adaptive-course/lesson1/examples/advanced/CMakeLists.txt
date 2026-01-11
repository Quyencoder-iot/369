# CMakeLists.txt - Ví dụ Advanced
cmake_minimum_required(VERSION 3.10)
project(hello_someip_advanced)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Find packages
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system log)

# Common include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VSOMEIP3_INCLUDE_DIRS}
)

# Service executable
add_executable(hello_service_advanced 
    hello_service_advanced.cpp
)

target_link_libraries(hello_service_advanced 
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# Client executable  
add_executable(hello_client_advanced
    hello_client_advanced.cpp
)

target_link_libraries(hello_client_advanced
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# Installation
install(TARGETS hello_service_advanced hello_client_advanced
    RUNTIME DESTINATION bin
)

install(FILES vsomeip_advanced.json
    DESTINATION etc
    RENAME vsomeip.json
)
