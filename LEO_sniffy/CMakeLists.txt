cmake_minimum_required(VERSION 3.21)

project(LEO_sniffy LANGUAGES C CXX)

# Prefer a modern standard (Qt 6 itself is built with >= C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable Qt's automatic processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    PrintSupport
    SerialPort
    Charts
    OpenGLWidgets
    Network
)

# Use Qt's standard project setup when available (Qt >= 6.3)
if(COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
endif()

# Collect sources (cpp + c), UI files, and resources
file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB_RECURSE C_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
file(GLOB_RECURSE UI_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)
# Add headers to target sources so AUTMOC sees Q_OBJECT macros
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

set(QRC_FILES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/graphics.qrc")
    list(APPEND QRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/graphics.qrc")
endif()

# Create the application target
if(COMMAND qt_add_executable)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${CPP_FILES}
        ${C_FILES}
        ${UI_FILES}
        ${HDR_FILES}
        ${QRC_FILES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${CPP_FILES}
        ${C_FILES}
        ${UI_FILES}
        ${HDR_FILES}
        ${QRC_FILES}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Mirror qmake defines
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_NO_FOREACH
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::SerialPort
    Qt6::Charts
    Qt6::OpenGLWidgets
    Qt6::Network
)

# Windows GUI app (no console window)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(COMMAND qt_finalize_executable)
    qt_finalize_executable(${PROJECT_NAME})
endif()
