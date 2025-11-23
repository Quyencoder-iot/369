cmake_minimum_required(VERSION 3.16)
project(QtMVCPointers VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt6 (hoặc Qt5)
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

message(STATUS "Đang sử dụng Qt${QT_VERSION_MAJOR}")

# Tự động xử lý MOC (Meta-Object Compiler)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Các executable cho từng ví dụ

# Ví dụ 1: Simple Model
add_executable(simple_model_example
    simple_model_example.cpp
)
target_link_libraries(simple_model_example
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Ví dụ 2: Custom Delegate
add_executable(custom_delegate_example
    custom_delegate_example.cpp
)
target_link_libraries(custom_delegate_example
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Ví dụ 3: Pointer Patterns
add_executable(pointer_patterns
    pointer_patterns.cpp
)
target_link_libraries(pointer_patterns
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
)

# In hướng dẫn build
message(STATUS "")
message(STATUS "🚀 HƯỚNG DẪN BUILD:")
message(STATUS "   mkdir build && cd build")
message(STATUS "   cmake ..")
message(STATUS "   make")
message(STATUS "")
message(STATUS "📝 Chạy các chương trình:")
message(STATUS "   ./simple_model_example")
message(STATUS "   ./custom_delegate_example")
message(STATUS "   ./pointer_patterns")
message(STATUS "")
