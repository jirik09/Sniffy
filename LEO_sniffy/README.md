# LEO Sniffy

LEO Sniffy is a powerful software-defined instrument interface designed for STM32 Nucleo boards. It transforms your Nucleo board into a versatile lab instrument, allowing you to capture signals, generate waveforms, and analyze data in real-time through a modern Qt-based interface.

## Features

- **Device Detection**: Automatically detects connected STM32 Nucleo boards via ST-Link.
- **Real-time Visualization**: High-performance charting for signal analysis.
- **Module Support**: Configurable modules for Oscilloscope, Signal Generator, PWM, and more.
- **Cross-Platform**: Available for Windows and Linux.

## Installation

### Windows

1.  Download the latest installer (`LEO_sniffy-x.x.x-win64.exe`) from the [Releases](https://github.com/StartYourPath/LEO_sniffy/releases) page.
2.  Run the installer and follow the on-screen instructions.
3.  **Drivers**:
    *   The application requires ST-Link USB drivers.
    *   If you have used your Nucleo board on this computer before (with STM32CubeIDE, Keil, etc.), the drivers are already installed.
    *   If the device is not detected, download and install the **STSW-LINK009** driver from [ST.com](https://www.st.com/en/development-tools/stsw-link009.html).

### Linux (Ubuntu/Debian)

1.  Download the latest Debian package (`LEO_sniffy-x.x.x-Linux.deb`) from the [Releases](https://github.com/StartYourPath/LEO_sniffy/releases) page.
2.  Install the package using `apt` or `dpkg`:
    ```bash
    sudo apt install ./LEO_sniffy-*-Linux.deb
    ```
    *(Note: Using `apt` is recommended as it automatically resolves dependencies like `libusb` and `libstlink`).*

#### Linux Permissions (udev rules)
To access the ST-Link device without `sudo` privileges, you must ensure the correct udev rules are installed.

If you installed the `stlink-tools` or `libstlink` package via your package manager, these rules might already be present. If you cannot connect to the device:
1.  Copy the stlink udev rules to `/etc/udev/rules.d/`. You can find standard rules in the [stlink-org/stlink](https://github.com/stlink-org/stlink/tree/master/config/udev/rules.d) repository.
2.  Reload the rules:
    ```bash
    sudo udevadm control --reload-rules
    sudo udevadm trigger
    ```

## Usage

1.  **Connect Hardware**: Plug your STM32 Nucleo board into your computer via USB.
2.  **Launch App**: Open **LEO Sniffy** from your desktop or start menu.
3.  **Connect**:
    *   Click the **Connect** button in the toolbar.
    *   Select your device from the detected list.
4.  **Operate**:
    *   Use the dashboard to enable/disable modules.
    *   Configure parameters (Frequency, Amplitude, etc.) in the settings panel.
    *   View real-time data on the main plot area.

## Building from Source

### Prerequisites
- Qt 6.2 or later
- CMake 3.21+
- Ninja (optional, but recommended)
- **Linux**: `libusb-1.0-0-dev`, `libstlink-dev`, `build-essential`

### Build Steps

```bash
git clone --recursive https://github.com/StartYourPath/LEO_sniffy.git
cd LEO_sniffy
mkdir build && cd build
cmake ..
cmake --build .
```
