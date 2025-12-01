cmake_minimum_required(VERSION 3.16)
project(LayerShellQtExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Wayland)

# Tìm LayerShellQt
find_package(LayerShellQt REQUIRED)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Top Panel Example
add_executable(top_panel top_panel.cpp)
target_link_libraries(top_panel
    Qt6::Core
    Qt6::Gui
    Qt6::Wayland
    LayerShellQt::LayerShellQt
)

# Notification Example
add_executable(notification notification.cpp)
target_link_libraries(notification
    Qt6::Core
    Qt6::Gui
    Qt6::Wayland
    LayerShellQt::LayerShellQt
)

# Bottom Dock Example
add_executable(bottom_dock bottom_dock.cpp)
target_link_libraries(bottom_dock
    Qt6::Core
    Qt6::Gui
    Qt6::Wayland
    LayerShellQt::LayerShellQt
)
