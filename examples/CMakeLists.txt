# CMakeLists.txt cho các ví dụ Qt Model/View

cmake_minimum_required(VERSION 3.16)
project(QtModelExamples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Bật tự động MOC
set(CMAKE_AUTOMOC ON)

# Ví dụ 1: List Model
add_executable(example1_list 01_list_model_example.cpp)
target_link_libraries(example1_list Qt6::Core Qt6::Widgets)

# Ví dụ 2: Table Model
add_executable(example2_table 02_table_model_example.cpp)
target_link_libraries(example2_table Qt6::Core Qt6::Widgets)

# Ví dụ 3: Tree Model
add_executable(example3_tree 03_tree_model_example.cpp)
target_link_libraries(example3_tree Qt6::Core Qt6::Widgets)

# Ví dụ 4: Proxy Model
add_executable(example4_proxy 04_proxy_model_example.cpp)
target_link_libraries(example4_proxy Qt6::Core Qt6::Widgets)

# Ví dụ 5: SQL Models
add_executable(example5_sql 05_qsql_model_example.cpp)
target_link_libraries(example5_sql Qt6::Core Qt6::Widgets Qt6::Sql)

# Ví dụ 6: Complete Example
add_executable(example6_complete 06_complete_example.cpp)
target_link_libraries(example6_complete Qt6::Core Qt6::Widgets Qt6::Sql)
