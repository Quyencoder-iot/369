cmake_minimum_required(VERSION 3.16)
project(LayerShellQt-Examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tìm các package cần thiết
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(LayerShellQt REQUIRED)

# Example 1: Top Panel
add_executable(top_panel top_panel.cpp)
target_link_libraries(top_panel PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    LayerShellQt::Interface
)

# Example 2: Floating Dock
add_executable(floating_dock floating_dock.cpp)
target_link_libraries(floating_dock PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    LayerShellQt::Interface
)

# Example 3: Desktop Widget
add_executable(desktop_widget desktop_widget.cpp)
target_link_libraries(desktop_widget PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    LayerShellQt::Interface
)

# Example 4: Notification
add_executable(notification notification.cpp)
target_link_libraries(notification PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    LayerShellQt::Interface
)
