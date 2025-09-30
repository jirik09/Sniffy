# Building LEO_sniffy with CMake (Qt 6)

This project has been migrated from qmake to CMake for Qt 6.

## Prerequisites
- Qt 6.2+ with the following modules: Core, Gui, Widgets, PrintSupport, SerialPort, Charts, OpenGLWidgets, Network
- CMake 3.21+
- A C++17 compiler (MSVC 2019/2022, MinGW, or Clang)

## Configure and build (Windows, PowerShell)

1. Make sure your Qt toolchain is available. If using the Qt Online Installer, open the corresponding "Qt 6.x.x (MSVC/MinGW)" Developer Command Prompt, or set CMAKE_PREFIX_PATH to your Qt installation, e.g.:
   - MSVC example: `C:/Qt/6.6.2/msvc2019_64/lib/cmake`
   - MinGW example: `C:/Qt/6.6.2/mingw_64/lib/cmake`

2. Configure and build:

```powershell
# From repository root or the LEO_sniffy folder
cd /<path to sniffy>/LEO_sniffy

# Create out-of-source build
mkdir build
cd build

# Point CMake to Qt if not already in environment
# cmake -G "Ninja" -DCMAKE_PREFIX_PATH="C:/Qt/6.6.2/msvc2019_64/lib/cmake" ..
cmake -G "Ninja" ..
cmake --build . --config Release
```

3. Run the app:
```powershell
./LEO_sniffy.exe
```

If the executable fails to start due to missing Qt runtime DLLs, run `windeployqt` from your Qt bin directory targeting `LEO_sniffy.exe`, or build/install with a CPack/deployment step of your choice.

## Notes
- CMake auto-discovers sources (*.cpp, *.c), forms (*.ui), and the Qt resource file `graphics.qrc`.
- Additional Qt modules can be added by extending the `find_package(Qt6 ... COMPONENTS ...)` list and `target_link_libraries`.
- Compiler definitions previously set in qmake are mirrored: `QT_DEPRECATED_WARNINGS`, `QT_NO_FOREACH`.