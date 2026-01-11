cmake_minimum_required(VERSION 3.10)
project(qt_simple_client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(vsomeip3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread log)

add_executable(qt_simple_client
    main.cpp
    mainwindow.cpp
    mainwindow.h
    serviceclient.cpp
    serviceclient.h
)

target_link_libraries(qt_simple_client
    Qt5::Widgets
    Qt5::Core
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)
