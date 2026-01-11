cmake_minimum_required(VERSION 3.10)
project(qt_dashboard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

# Sources
set(SOURCES
    main.cpp
    dashboard/dashboardwindow.cpp
    widgets/speedgauge.cpp
    widgets/batterywidget.cpp
    service/dashboardproxy.cpp
)

set(HEADERS
    dashboard/dashboardwindow.h
    widgets/speedgauge.h
    widgets/batterywidget.h
    service/dashboardproxy.h
    service/datatypes.h
)

# Create executable
add_executable(qt_dashboard ${SOURCES} ${HEADERS})

target_include_directories(qt_dashboard PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(qt_dashboard
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# Copy vsomeip config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2/vsomeip.json
    ${CMAKE_BINARY_DIR}/vsomeip.json
    COPYONLY
)

# Install
install(TARGETS qt_dashboard DESTINATION bin)
