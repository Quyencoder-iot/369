cmake_minimum_required(VERSION 3.16)
project(QtSourceLearning)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# Enable automatic MOC
set(CMAKE_AUTOMOC ON)

# Add executable
add_executable(${PROJECT_NAME}
    VI_DU_THUC_TE.cpp
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
)

# Set C++ standard
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# Enable debug output
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_MESSAGELOGCONTEXT
)
