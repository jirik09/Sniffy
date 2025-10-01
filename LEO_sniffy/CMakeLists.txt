cmake_minimum_required(VERSION 3.21)


project(LEO_sniffy LANGUAGES C CXX)

# Use -O2 for release builds
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Prefer a modern standard (Qt 6 itself is built with >= C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Speed options (togglable)
option(SNIFFY_ENABLE_UNITY "Enable unity builds for faster compilation" OFF)
set(SNIFFY_UNITY_BATCH_SIZE "8" CACHE STRING "How many sources per unity translation unit")
option(SNIFFY_USE_PCH "Use precompiled headers" ON)
option(SNIFFY_FAST_RCC "Disable RCC compression for faster resource build" ON)

# Use ccache/sccache if available
find_program(SNIFFY_CCACHE_PROGRAM ccache)
find_program(SNIFFY_SCCACHE_PROGRAM sccache)
if(SNIFFY_CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${SNIFFY_CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER   ${SNIFFY_CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${SNIFFY_CCACHE_PROGRAM})
elseif(SNIFFY_SCCACHE_PROGRAM)
    message(STATUS "Using sccache: ${SNIFFY_SCCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER   ${SNIFFY_SCCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${SNIFFY_SCCACHE_PROGRAM})
endif()

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
# Add headers to target sources for proper dependency tracking and IDE support
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

# Enable unity build (can significantly reduce compile time)
if(SNIFFY_ENABLE_UNITY)
    # On MinGW/GNU, very large unity objects may hit 'too many sections'. Use a smaller batch.
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if(NOT SNIFFY_UNITY_BATCH_SIZE)
            # MinGW's COFF object format has section limits; keep unity groups very small
            set(SNIFFY_UNITY_BATCH_SIZE 2)
        endif()
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        UNITY_BUILD TRUE
        UNITY_BUILD_BATCH_SIZE ${SNIFFY_UNITY_BATCH_SIZE}
    )
endif()

# Speed up RCC by disabling compression (useful in Debug)
if(SNIFFY_FAST_RCC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY AUTORCC_OPTIONS "-no-compress")
endif()

# Precompiled headers for commonly used APIs
if(SNIFFY_USE_PCH)
    # PCH only for C++ sources
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/cmake/pch.hpp>
    )
endif()

if(COMMAND qt_finalize_executable)
    qt_finalize_executable(${PROJECT_NAME})
endif()

# MinGW (GNU) assembler may hit 'too many sections'; enable big-obj to raise limits
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
endif()
