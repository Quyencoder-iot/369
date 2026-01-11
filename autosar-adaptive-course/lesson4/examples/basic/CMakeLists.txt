# CMakeLists.txt - Basic project
cmake_minimum_required(VERSION 3.10)
project(autosar_dashboard_basic VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Copy lesson2 basic files as source
# In real project, organize source files properly

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
