cmake_minimum_required(VERSION 3.16)
project(LayerShellQt-Examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find dependencies
find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets Quick Qml)
find_package(LayerShellQt REQUIRED)

# Helper function to create example executables
function(add_example name source)
    add_executable(${name} ${source})
    target_link_libraries(${name}
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        LayerShellQt::Interface
    )
endfunction()

# Build all examples
add_example(01-simple-panel 01-simple-panel.cpp)
add_example(02-notification 02-notification.cpp)
add_example(03-desktop-widget 03-desktop-widget.cpp)
add_example(04-animated-wallpaper 04-animated-wallpaper.cpp)
add_example(05-multi-monitor-panel 05-multi-monitor-panel.cpp)

# QML example (needs Qt Quick)
qt5_add_resources(QML_RESOURCES resources.qrc)
add_executable(06-qml-multi-monitor-panel 
    06-qml-multi-monitor-panel.cpp 
    ${QML_RESOURCES}
)
target_link_libraries(06-qml-multi-monitor-panel
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    LayerShellQt::Interface
)

# Installation
install(TARGETS 
    01-simple-panel
    02-notification
    03-desktop-widget
    04-animated-wallpaper
    05-multi-monitor-panel
    06-qml-multi-monitor-panel
    DESTINATION bin
)
