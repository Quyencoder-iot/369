# CMakeLists.txt - Ví dụ Basic
cmake_minimum_required(VERSION 3.10)
project(hello_someip_basic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm vsomeip
find_package(vsomeip3 REQUIRED)

# Tìm Boost threads
find_package(Boost REQUIRED COMPONENTS thread system)

# Service executable
add_executable(hello_service_basic hello_service_basic.cpp)
target_link_libraries(hello_service_basic 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)

# Client executable
add_executable(hello_client_basic hello_client_basic.cpp)
target_link_libraries(hello_client_basic 
    vsomeip3 
    ${Boost_LIBRARIES} 
    pthread
)
