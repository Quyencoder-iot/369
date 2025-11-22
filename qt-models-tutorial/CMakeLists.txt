cmake_minimum_required(VERSION 3.16)
project(QtModelsTutorial)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Main application
add_executable(qt_models_demo
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
)

# Examples executables
add_executable(list_model_example
    examples/list_model_example.cpp
)

add_executable(table_model_example
    examples/table_model_example.cpp
)

add_executable(tree_model_example
    examples/tree_model_example.cpp
)

add_executable(proxy_model_example
    examples/proxy_model_example.cpp
)

add_executable(sql_model_example
    examples/sql_model_example.cpp
)

# Link libraries
target_link_libraries(qt_models_demo Qt6::Core Qt6::Widgets Qt6::Sql)
target_link_libraries(list_model_example Qt6::Core Qt6::Widgets)
target_link_libraries(table_model_example Qt6::Core Qt6::Widgets)
target_link_libraries(tree_model_example Qt6::Core Qt6::Widgets)
target_link_libraries(proxy_model_example Qt6::Core Qt6::Widgets)
target_link_libraries(sql_model_example Qt6::Core Qt6::Widgets Qt6::Sql)