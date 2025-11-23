# CMakeLists.txt cho các ví dụ Qt Model-View-Delegate

cmake_minimum_required(VERSION 3.16)
project(QtModelViewExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Bật MOC tự động
set(CMAKE_AUTOMOC ON)

# Ví dụ 1: Basic Example
add_executable(basic_example basic_example.cpp)
target_link_libraries(basic_example Qt6::Core Qt6::Widgets)

# Ví dụ 2: Delegate Example
add_executable(delegate_example delegate_example.cpp)
target_link_libraries(delegate_example Qt6::Core Qt6::Widgets)

# Ví dụ 3: Multi-View Example
add_executable(multi_view_example multi_view_example.cpp)
target_link_libraries(multi_view_example Qt6::Core Qt6::Widgets)
