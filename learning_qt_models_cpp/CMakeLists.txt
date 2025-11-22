cmake_minimum_required(VERSION 3.16)

project(QtModelsTutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

set(SOURCES
    main.cpp
    custom_list_model.cpp
    custom_list_model.h
    custom_table_model.cpp
    custom_table_model.h
    custom_tree_model.cpp
    custom_tree_model.h
)

add_executable(QtModelsTutorial ${SOURCES})

target_link_libraries(QtModelsTutorial PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
