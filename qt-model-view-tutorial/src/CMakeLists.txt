cmake_minimum_required(VERSION 3.16)
project(QtModelViewPointerTutorial)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 (or Qt5 if Qt6 not available)
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
endif()

# Enable MOC (Meta Object Compiler)
set(CMAKE_AUTOMOC ON)

# Add executable
add_executable(model_view_demo
    main.cpp
    mainwindow.cpp
    mainwindow.h
    custom_model.cpp
    custom_model.h
    custom_delegate.cpp
    custom_delegate.h
    pointer_manager.cpp
    pointer_manager.h
)

# Link Qt libraries
if(Qt6_FOUND)
    target_link_libraries(model_view_demo Qt6::Core Qt6::Widgets)
else()
    target_link_libraries(model_view_demo Qt5::Core Qt5::Widgets)
endif()

# Set executable properties
set_target_properties(model_view_demo PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)