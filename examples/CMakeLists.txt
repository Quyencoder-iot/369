cmake_minimum_required(VERSION 3.16)
project(QtModelsExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql)

# Enable MOC
set(CMAKE_AUTOMOC ON)

# List Model Example
add_executable(list_model_example list_model_example.cpp)
target_link_libraries(list_model_example Qt5::Core Qt5::Widgets)

# Table Model Example
add_executable(table_model_example table_model_example.cpp)
target_link_libraries(table_model_example Qt5::Core Qt5::Widgets)

# Tree Model Example
add_executable(tree_model_example tree_model_example.cpp)
target_link_libraries(tree_model_example Qt5::Core Qt5::Widgets)

# Proxy Model Example
add_executable(proxy_model_example proxy_model_example.cpp)
target_link_libraries(proxy_model_example Qt5::Core Qt5::Widgets)

# SQL Model Example
add_executable(sql_model_example sql_model_example.cpp)
target_link_libraries(sql_model_example Qt5::Core Qt5::Widgets Qt5::Sql)

# Complete Example
add_executable(complete_example complete_example.cpp)
target_link_libraries(complete_example Qt5::Core Qt5::Widgets Qt5::Sql)
