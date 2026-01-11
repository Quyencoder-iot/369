cmake_minimum_required(VERSION 3.10)
project(autosar_dashboard VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_SERVICE "Build Dashboard Service" ON)
option(BUILD_CLIENT "Build Console Client" ON)
option(BUILD_QT_UI "Build Qt Dashboard" ON)
option(BUILD_TESTS "Build Unit Tests" OFF)

# Find common dependencies
find_package(vsomeip3 QUIET)
find_package(Boost QUIET COMPONENTS system thread log)

if(NOT vsomeip3_FOUND)
    message(WARNING "vsomeip3 not found. Install vsomeip to build.")
endif()

if(Boost_FOUND AND vsomeip3_FOUND)
    # Include directories
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    
    # Dashboard Service
    if(BUILD_SERVICE)
        add_executable(dashboard_service 
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2/dashboard_service.cpp
        )
        target_include_directories(dashboard_service PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2
        )
        target_link_libraries(dashboard_service 
            vsomeip3 
            ${Boost_LIBRARIES}
            pthread
        )
    endif()
    
    # Console Client
    if(BUILD_CLIENT)
        add_executable(dashboard_client
            ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2/dashboard_client.cpp
        )
        target_link_libraries(dashboard_client
            vsomeip3
            ${Boost_LIBRARIES}
            pthread
        )
    endif()
    
    # Qt Dashboard
    if(BUILD_QT_UI)
        find_package(Qt5 QUIET COMPONENTS Widgets Core Gui)
        if(Qt5_FOUND)
            set(CMAKE_AUTOMOC ON)
            
            add_executable(qt_dashboard
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2/main.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2/dashboard/dashboardwindow.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2/widgets/speedgauge.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2/widgets/batterywidget.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2/service/dashboardproxy.cpp
            )
            target_include_directories(qt_dashboard PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson3/example2
            )
            target_link_libraries(qt_dashboard
                Qt5::Widgets
                Qt5::Core
                Qt5::Gui
                vsomeip3
                ${Boost_LIBRARIES}
                pthread
            )
        else()
            message(WARNING "Qt5 not found, skipping Qt UI build")
        endif()
    endif()
endif()

# Copy configuration files
file(COPY 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2/database.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lesson2/example2/vsomeip.json
    DESTINATION ${CMAKE_BINARY_DIR}
)

# CPack for packaging
set(CPACK_PACKAGE_NAME "autosar-dashboard")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "AUTOSAR Adaptive Dashboard Demo")
set(CPACK_GENERATOR "TGZ")
include(CPack)
