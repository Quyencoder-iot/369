cmake_minimum_required(VERSION 3.10)
project(qt_client_basic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

# Executable
add_executable(qt_client_basic 
    qt_client_basic.cpp
)

target_link_libraries(qt_client_basic 
    Qt5::Core
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)
