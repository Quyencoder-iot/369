cmake_minimum_required(VERSION 3.14)
project(QtModelsQMLExamples VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm Qt5 hoặc Qt6
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

# Enable automatic MOC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ============================================
# Example 1: Todo List (QML + C++)
# ============================================
add_executable(qml_todo_list
    main_todo.cpp
    models/TodoListModel.h
    models/TodoListModel.cpp
    qml.qrc
)

target_link_libraries(qml_todo_list
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# Example 2: Student Table (QML + C++)
# ============================================
add_executable(qml_student_table
    main_student.cpp
    models/StudentTableModel.h
    models/StudentTableModel.cpp
    qml.qrc
)

target_link_libraries(qml_student_table
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# Install
# ============================================
install(TARGETS
    qml_todo_list
    qml_student_table
    RUNTIME DESTINATION bin
)

# ============================================
# Build info
# ============================================
message(STATUS "========================================")
message(STATUS "Building Qt Models QML Examples")
message(STATUS "Qt version: ${QT_VERSION}")
message(STATUS "Qt major: ${QT_VERSION_MAJOR}")
message(STATUS "========================================")
