cmake_minimum_required(VERSION 3.16)
project(QtModelsExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Example 1: String List Model
add_executable(string_list_example
    examples/1_string_list_model.cpp
)
target_link_libraries(string_list_example Qt6::Core Qt6::Widgets)

# Example 2: Standard Item Model (Table)
add_executable(table_model_example
    examples/2_table_model.cpp
)
target_link_libraries(table_model_example Qt6::Core Qt6::Widgets)

# Example 3: Tree Model
add_executable(tree_model_example
    examples/3_tree_model.cpp
)
target_link_libraries(tree_model_example Qt6::Core Qt6::Widgets)

# Example 4: Proxy Model
add_executable(proxy_model_example
    examples/4_proxy_model.cpp
)
target_link_libraries(proxy_model_example Qt6::Core Qt6::Widgets)

# Example 5: SQL Models
add_executable(sql_model_example
    examples/5_sql_models.cpp
)
target_link_libraries(sql_model_example Qt6::Core Qt6::Widgets Qt6::Sql)

# Example 6: Complete Demo
add_executable(complete_demo
    examples/6_complete_demo.cpp
)
target_link_libraries(complete_demo Qt6::Core Qt6::Widgets Qt6::Sql)