cmake_minimum_required(VERSION 3.16)
project(QtModelsExamples VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Enable automatic MOC, UIC, and RRC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Example 1: List Model
add_executable(example_01_list_model
    01_list_model.cpp
)
target_link_libraries(example_01_list_model
    Qt6::Core
    Qt6::Widgets
)

# Example 2: Table Model
add_executable(example_02_table_model
    02_table_model.cpp
)
target_link_libraries(example_02_table_model
    Qt6::Core
    Qt6::Widgets
)

# Example 3: Tree Model
add_executable(example_03_tree_model
    03_tree_model.cpp
)
target_link_libraries(example_03_tree_model
    Qt6::Core
    Qt6::Widgets
)

# Example 4: Proxy Model
add_executable(example_04_proxy_model
    04_proxy_model.cpp
)
target_link_libraries(example_04_proxy_model
    Qt6::Core
    Qt6::Widgets
)

# Example 5: QSql Models
add_executable(example_05_qsql_models
    05_qsql_models.cpp
)
target_link_libraries(example_05_qsql_models
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Example 6: Complete Example
add_executable(example_06_complete
    06_complete_example.cpp
)
target_link_libraries(example_06_complete
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Install targets
install(TARGETS
    example_01_list_model
    example_02_table_model
    example_03_tree_model
    example_04_proxy_model
    example_05_qsql_models
    example_06_complete
    RUNTIME DESTINATION bin
)

# Display build info
message(STATUS "Building Qt Models Examples")
message(STATUS "Qt version: ${Qt6_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
