cmake_minimum_required(VERSION 3.5)

project(QtModelsQmlComplete VERSION 1.0 LANGUAGES CXX)

# ============================================
# C++ STANDARD
# ============================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# Qt5/Qt6 AUTO-DETECTION
# ============================================
# Try Qt6 first, fallback to Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick Sql)

message(STATUS "✅ Using Qt${QT_VERSION_MAJOR}")

# ============================================
# Qt MOC/RCC AUTO-PROCESSING
# ============================================
set(CMAKE_AUTOMOC ON)  # Auto-process Q_OBJECT
set(CMAKE_AUTORCC ON)  # Auto-process .qrc files
set(CMAKE_AUTOUIC ON)  # Auto-process .ui files (not used here)

# ============================================
# SOURCE FILES
# ============================================

# Model implementations (shared across all examples)
set(MODEL_SOURCES
    models/TodoListModel.cpp
    models/StudentTableModel.cpp
    models/FileSystemTreeModel.cpp
    models/EmployeeSqlModel.cpp
    models/ProductFilterProxyModel.cpp
)

set(MODEL_HEADERS
    models/TodoListModel.h
    models/StudentTableModel.h
    models/FileSystemTreeModel.h
    models/EmployeeSqlModel.h
    models/ProductFilterProxyModel.h
)

# QML resources (shared across all examples)
set(QML_RESOURCES
    qml.qrc
)

# ============================================
# EXAMPLE 1: TODO LIST
# ============================================
add_executable(example_01_todo
    main_01_todo.cpp
    models/TodoListModel.cpp
    models/TodoListModel.h
    ${QML_RESOURCES}
)

target_link_libraries(example_01_todo
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# EXAMPLE 2: STUDENT TABLE
# ============================================
add_executable(example_02_student
    main_02_student.cpp
    models/StudentTableModel.cpp
    models/StudentTableModel.h
    ${QML_RESOURCES}
)

target_link_libraries(example_02_student
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# EXAMPLE 3: FILE SYSTEM TREE
# ============================================
add_executable(example_03_tree
    main_03_tree.cpp
    models/FileSystemTreeModel.cpp
    models/FileSystemTreeModel.h
    ${QML_RESOURCES}
)

target_link_libraries(example_03_tree
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# EXAMPLE 4: EMPLOYEE SQL
# ============================================
add_executable(example_04_sql
    main_04_sql.cpp
    models/EmployeeSqlModel.cpp
    models/EmployeeSqlModel.h
    ${QML_RESOURCES}
)

target_link_libraries(example_04_sql
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Sql  # SQL module required
)

# ============================================
# EXAMPLE 5: PRODUCT FILTER PROXY
# ============================================
add_executable(example_05_proxy
    main_05_proxy.cpp
    models/ProductFilterProxyModel.cpp
    models/ProductFilterProxyModel.h
    ${QML_RESOURCES}
)

target_link_libraries(example_05_proxy
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
)

# ============================================
# OPTIONAL: BUILD ALL TARGET
# ============================================
add_custom_target(build_all
    DEPENDS
        example_01_todo
        example_02_student
        example_03_tree
        example_04_sql
        example_05_proxy
)

# ============================================
# INSTALLATION (Optional)
# ============================================
install(TARGETS
    example_01_todo
    example_02_student
    example_03_tree
    example_04_sql
    example_05_proxy
    DESTINATION bin
)

# ============================================
# BUILD INSTRUCTIONS
# ============================================
message(STATUS "")
message(STATUS "============================================")
message(STATUS "Qt Models QML Complete Examples")
message(STATUS "============================================")
message(STATUS "Qt Version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "BUILD COMMANDS:")
message(STATUS "  mkdir build && cd build")
message(STATUS "  cmake ..")
message(STATUS "  make")
message(STATUS "")
message(STATUS "RUN EXAMPLES:")
message(STATUS "  ./example_01_todo")
message(STATUS "  ./example_02_student")
message(STATUS "  ./example_03_tree")
message(STATUS "  ./example_04_sql")
message(STATUS "  ./example_05_proxy")
message(STATUS "============================================")
message(STATUS "")

# ============================================
# 📚 SUMMARY - CMakeLists.txt
# ============================================
#
# 1. PROJECT STRUCTURE:
#    qml_complete/
#      ├─ CMakeLists.txt (this file)
#      ├─ qml.qrc (QML resources)
#      ├─ main_01_todo.cpp
#      ├─ main_02_student.cpp
#      ├─ main_03_tree.cpp
#      ├─ main_04_sql.cpp
#      ├─ main_05_proxy.cpp
#      ├─ models/
#      │   ├─ TodoListModel.h/cpp
#      │   ├─ StudentTableModel.h/cpp
#      │   ├─ FileSystemTreeModel.h/cpp
#      │   ├─ EmployeeSqlModel.h/cpp
#      │   └─ ProductFilterProxyModel.h/cpp
#      └─ qml/
#          ├─ TodoListView.qml
#          ├─ StudentTableView.qml
#          ├─ FileSystemTreeView.qml
#          ├─ EmployeeSqlView.qml
#          └─ ProductFilterProxyView.qml
#
# 2. CMAKE FEATURES:
#    ✅ Qt5/Qt6 auto-detection
#    ✅ CMAKE_AUTOMOC - Auto Q_OBJECT processing
#    ✅ CMAKE_AUTORCC - Auto .qrc processing
#    ✅ Separate executable per example
#    ✅ Shared QML resources
#
# 3. Qt MODULES USED:
#    - Qt::Core - Core functionality
#    - Qt::Quick - QML/QtQuick engine
#    - Qt::Sql - Database (example 4 only)
#
# 4. BUILD PROCESS:
#    cmake ..
#      ↓
#    Generate Makefiles
#      ↓
#    Process .qrc (rcc) → C++ code
#      ↓
#    Process Q_OBJECT (moc) → meta-object code
#      ↓
#    Compile C++ sources
#      ↓
#    Link executables
#      ↓
#    5 executable files ready
#
# 5. Qt5 vs Qt6 DIFFERENCES:
#    Qt5:
#      - find_package(Qt5 COMPONENTS Core Quick Sql)
#      - target_link_libraries(app Qt5::Core Qt5::Quick)
#    
#    Qt6:
#      - find_package(Qt6 COMPONENTS Core Quick Sql)
#      - target_link_libraries(app Qt6::Core Qt6::Quick)
#    
#    This CMakeLists:
#      - Auto-detects version
#      - Uses Qt${QT_VERSION_MAJOR}::Module
#      - Works with both!
#
# 6. COMMON CMAKE VARIABLES:
#    CMAKE_CXX_STANDARD - C++ version (11, 14, 17, 20)
#    CMAKE_AUTOMOC - Enable moc processing
#    CMAKE_AUTORCC - Enable rcc processing
#    PROJECT_SOURCE_DIR - Root directory
#    CMAKE_CURRENT_SOURCE_DIR - Current CMakeLists.txt directory
#
# 7. WHY SEPARATE EXECUTABLES?
#    ✅ Independent examples
#    ✅ Easier testing
#    ✅ Clear separation
#    ✅ Smaller binaries (only needed code)
#    
#    Alternative: Single executable with command-line args
#
# 8. RESOURCE HANDLING:
#    qml.qrc listed in add_executable()
#      ↓
#    CMAKE_AUTORCC processes it
#      ↓
#    rcc generates qrc_qml.cpp
#      ↓
#    Compiled and linked
#      ↓
#    QML files embedded in executable
#
# ============================================
