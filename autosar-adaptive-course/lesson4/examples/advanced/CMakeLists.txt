# ============================================================================
# CMakeLists.txt - VÍ DỤ 2: Project đóng gói hoàn chỉnh
# ============================================================================
#
# Cấu trúc project:
# ├── CMakeLists.txt (this file)
# ├── src/
# │   ├── service/
# │   │   └── dashboard_service.cpp
# │   └── client/
# │       ├── main.cpp
# │       ├── mainwindow.cpp
# │       └── servicebridge.cpp
# ├── include/
# │   ├── ara_types.hpp
# │   ├── json_parser.hpp
# │   ├── mainwindow.h
# │   └── servicebridge.h
# ├── config/
# │   ├── vsomeip.json
# │   └── vehicle_database.json
# └── packaging/
#     └── CMakeLists.txt
#

cmake_minimum_required(VERSION 3.10)

# ============================================================================
# PROJECT CONFIGURATION
# ============================================================================
project(autosar_adaptive_dashboard
    VERSION 1.0.0
    DESCRIPTION "AUTOSAR Adaptive Dashboard Demo"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# OPTIONS
# ============================================================================
option(BUILD_QT_CLIENT "Build Qt GUI client" ON)
option(BUILD_CONSOLE_CLIENT "Build console client" ON)
option(BUILD_TESTS "Build tests" OFF)
option(ENABLE_LOGGING "Enable detailed logging" ON)

# ============================================================================
# DEPENDENCIES
# ============================================================================
message(STATUS "Finding dependencies...")

# vsomeip
find_package(vsomeip3 REQUIRED)
message(STATUS "  vsomeip3: FOUND")

# Boost
find_package(Boost REQUIRED COMPONENTS thread system log)
message(STATUS "  Boost: FOUND (${Boost_VERSION})")

# Qt5 (optional)
if(BUILD_QT_CLIENT)
    find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
    if(Qt5_FOUND)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)
        message(STATUS "  Qt5: FOUND (${Qt5_VERSION})")
    else()
        message(WARNING "  Qt5: NOT FOUND - Qt client will not be built")
        set(BUILD_QT_CLIENT OFF)
    endif()
endif()

# ============================================================================
# COMPILER FLAGS
# ============================================================================
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
    )
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O2)
    endif()
endif()

if(ENABLE_LOGGING)
    add_definitions(-DENABLE_LOGGING)
endif()

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ============================================================================
# COMMON LIBRARY
# ============================================================================
add_library(ara_common INTERFACE)
target_include_directories(ara_common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# DASHBOARD SERVICE
# ============================================================================
set(SERVICE_SOURCES
    src/service/dashboard_service.cpp
)

add_executable(dashboard_service ${SERVICE_SOURCES})
target_link_libraries(dashboard_service
    ara_common
    vsomeip3
    ${Boost_LIBRARIES}
    pthread
)

# ============================================================================
# CONSOLE CLIENT
# ============================================================================
if(BUILD_CONSOLE_CLIENT)
    set(CONSOLE_CLIENT_SOURCES
        src/client/console_client.cpp
    )
    
    add_executable(dashboard_client_console ${CONSOLE_CLIENT_SOURCES})
    target_link_libraries(dashboard_client_console
        ara_common
        vsomeip3
        ${Boost_LIBRARIES}
        pthread
    )
endif()

# ============================================================================
# QT CLIENT
# ============================================================================
if(BUILD_QT_CLIENT)
    set(QT_CLIENT_SOURCES
        src/client/main.cpp
        src/client/mainwindow.cpp
        src/client/servicebridge.cpp
    )
    
    set(QT_CLIENT_HEADERS
        include/mainwindow.h
        include/servicebridge.h
    )
    
    add_executable(dashboard_client_qt
        ${QT_CLIENT_SOURCES}
        ${QT_CLIENT_HEADERS}
    )
    
    target_link_libraries(dashboard_client_qt
        ara_common
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        vsomeip3
        ${Boost_LIBRARIES}
        pthread
    )
endif()

# ============================================================================
# CONFIGURATION FILES
# ============================================================================
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/vsomeip.json
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vsomeip.json
    COPYONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/vehicle_database.json
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vehicle_database.json
    COPYONLY
)

# ============================================================================
# INSTALLATION
# ============================================================================
include(GNUInstallDirs)

# Install binaries
install(TARGETS dashboard_service
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_CONSOLE_CLIENT)
    install(TARGETS dashboard_client_console
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

if(BUILD_QT_CLIENT)
    install(TARGETS dashboard_client_qt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# Install config files
install(DIRECTORY config/
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/autosar_dashboard
    FILES_MATCHING PATTERN "*.json"
)

# Install documentation
install(FILES README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# ============================================================================
# CPACK CONFIGURATION
# ============================================================================
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_VENDOR "AUTOSAR Adaptive Course")
set(CPACK_PACKAGE_CONTACT "course@example.com")

# DEB package settings
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-all-dev, libqt5widgets5")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

# RPM package settings
set(CPACK_RPM_PACKAGE_REQUIRES "boost-devel, qt5-qtbase")

include(CPack)

# ============================================================================
# SUMMARY
# ============================================================================
message(STATUS "")
message(STATUS "=== Build Configuration ===")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Qt client: ${BUILD_QT_CLIENT}")
message(STATUS "  Console client: ${BUILD_CONSOLE_CLIENT}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Logging: ${ENABLE_LOGGING}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
