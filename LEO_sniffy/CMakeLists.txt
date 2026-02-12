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
    "${CMAKE_CURRENT_SOURCE_DIR}/flasher/*.cpp"
)
list(FILTER CPP_FILES EXCLUDE REGEX "/(build|build_ninja|cmake-build-debug|CMakeFiles)/")
file(GLOB_RECURSE C_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
list(FILTER C_FILES EXCLUDE REGEX "/(build|build_ninja|cmake-build-debug|CMakeFiles)/")
file(GLOB_RECURSE UI_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.ui"
)
# Add headers to target sources for proper dependency tracking and IDE support
file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/flasher/*.h"
)

set(QRC_FILES)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/graphics.qrc")
    list(APPEND QRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/graphics.qrc")
endif()

# Windows Icon Resource
set(RC_FILES)
if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/leo_sniffy.rc")
    list(APPEND RC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/leo_sniffy.rc")
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
        ${RC_FILES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${CPP_FILES}
        ${C_FILES}
        ${UI_FILES}
        ${HDR_FILES}
        ${QRC_FILES}
        ${RC_FILES}
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

# --- ST-Link & LibUSB Integration ---
if(WIN32)
    set(LIBUSB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs/libusb")
    set(STLINK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libs/stlink")

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${LIBUSB_ROOT}/include"
        "${LIBUSB_ROOT}/include/libusb-1.0"
        "${STLINK_ROOT}/include"
    )

    # Link libraries
    if(MSVC)
        set(LIBUSB_LIB "${LIBUSB_ROOT}/lib/libusb-1.0.lib")
        set(STLINK_LIB "${STLINK_ROOT}/lib/libstlink.a") 
    else()
        # MinGW: Link directly against the DLL if .a is missing or incompatible
        set(LIBUSB_LIB "${LIBUSB_ROOT}/bin/libusb-1.0.dll") 
        set(STLINK_LIB "${STLINK_ROOT}/lib/libstlink.a")
    endif()

    set(EXTRA_LIBS ${LIBUSB_LIB} ${STLINK_LIB})

    # Copy DLLs to output directory
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LIBUSB_ROOT}/bin/libusb-1.0.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${STLINK_ROOT}/bin/libstlink.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${STLINK_ROOT}/config/chips"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/config/chips"
    )

elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
    
    # Try to find stlink library
    find_library(STLINK_LIB NAMES stlink stlink-static)
    if(NOT STLINK_LIB)
        message(WARNING "libstlink not found. Please install libstlink-dev.")
    else()
        message(STATUS "Found libstlink: ${STLINK_LIB}")
    endif()

    # Find stlink headers (typically in /usr/include/stlink/)
    find_path(STLINK_INCLUDE_DIR stlink.h
        PATH_SUFFIXES stlink
    )
    if(STLINK_INCLUDE_DIR)
        message(STATUS "Found stlink headers: ${STLINK_INCLUDE_DIR}")
    else()
        message(WARNING "stlink.h not found. Please install libstlink-dev.")
    endif()

    target_include_directories(${PROJECT_NAME} PRIVATE ${LIBUSB_INCLUDE_DIRS} ${STLINK_INCLUDE_DIR})
    set(EXTRA_LIBS ${LIBUSB_LIBRARIES} ${STLINK_LIB})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::SerialPort
    Qt6::Charts
    Qt6::OpenGLWidgets
    Qt6::Network
    ${EXTRA_LIBS}
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

# Deploy Qt dependencies on Windows
if(WIN32)
    find_package(Qt6Core REQUIRED)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE:${PROJECT_NAME}>"
            "--dir" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            "--no-translations"
            "--compiler-runtime"
            COMMENT "Deploying Qt dependencies using windeployqt..."
        )
    endif()
endif()

# MinGW (GNU) assembler may hit 'too many sections'; enable big-obj to raise limits
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj)
endif()

# --- Installation & Deployment ---
include(GNUInstallDirs)

# 1. Install the main executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

# 2. Install ST-Link & LibUSB DLLs and Configs
if(WIN32)
    # Install the required DLLs next to the executable
    install(FILES "${LIBUSB_ROOT}/bin/libusb-1.0.dll" DESTINATION bin)
    install(FILES "${STLINK_ROOT}/bin/libstlink.dll" DESTINATION bin)
    
    # Install ST-Link config folder structure (config/chips)
    install(DIRECTORY "${STLINK_ROOT}/config/chips" DESTINATION bin/config)
    
    # 3. Use windeployqt to automatically copy Qt DLLs during packaging
    # We re-find here to ensure variables are available in this scope/if block
    find_package(Qt6 COMPONENTS Core REQUIRED)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    if(WINDEPLOYQT_EXECUTABLE)
        install(CODE "
            message(STATUS \"Running windeployqt...\")
            execute_process(
                COMMAND \"${WINDEPLOYQT_EXECUTABLE}\"
                        --no-translations
                        --compiler-runtime
                        --no-opengl-sw
                        --dir \"\${CMAKE_INSTALL_PREFIX}/bin\"
                        \"\${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.exe\"
            )
        ")
    endif()
endif()

# --- CPack Setup (Installer Generation) ---
set(CPACK_PACKAGE_NAME "LEO_sniffy")
set(CPACK_PACKAGE_VENDOR "Sniffy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Sniffy LEO Interface for Nucleo Boards")
set(CPACK_PACKAGE_CONTACT "support@sniffy.com")

# License File (GPLv3)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")

# Set version from CMake project or manual
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

if(WIN32)
    # Use NSIS (Nullsoft Scriptable Install System) to create a .exe installer
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "LEO Sniffy")
    set(CPACK_NSIS_PACKAGE_NAME "LEO Sniffy Installer")
    
    # Icons for Installer
    set(WIN_ICON "${CMAKE_CURRENT_SOURCE_DIR}/graphics/logo.ico")
    if(EXISTS "${WIN_ICON}")
        set(CPACK_NSIS_MUI_ICON "${WIN_ICON}")
        set(CPACK_NSIS_MUI_UNIICON "${WIN_ICON}")
        # Add uninstall icon (optional, reuses same icon)
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\${PROJECT_NAME}.exe")
    endif()

    set(CPACK_NSIS_MODIFY_PATH ON) # Allow adding to PATH
    set(CPACK_NSIS_MENU_LINKS "bin/${PROJECT_NAME}.exe" "LEO Sniffy") # Add Start Menu shortcut
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sniffy Team")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON) # Auto-detect dependencies
endif()

include(CPack)
