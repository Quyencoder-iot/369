cmake_minimum_required(VERSION 3.10)
project(qt_dashboard_advanced)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)

# Source files
set(SOURCES
    main.cpp
    mainwindow.cpp
    servicebridge.cpp
)

set(HEADERS
    mainwindow.h
    servicebridge.h
)

# Executable
add_executable(qt_dashboard_advanced
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(qt_dashboard_advanced
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# Install
install(TARGETS qt_dashboard_advanced
    RUNTIME DESTINATION bin
)
