# Ball Trajectory Simulation

A cross-platform C++ application for simulating and visualizing ball trajectories with physics calculations, featuring a graphical interface built with ImGui and ImPlot.

![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)

## Features

- 🎯 Interactive trajectory simulation with real-time visualization
- 📊 Beautiful plotting with ImPlot
- 🌍 Multiple space object presets (Earth, Moon, Mars, etc.)
- 💨 Wind and atmospheric effects
- 💾 Save and load simulations
- 🖥️ Cross-platform (Linux, Windows, macOS)

## Quick Start

### Prerequisites

**System Dependencies:**
```bash
# Linux (Ubuntu/Debian)
sudo apt install cmake build-essential libglfw3-dev libgl1-mesa-dev git

# macOS
brew install cmake glfw

# Windows
# Use vcpkg: vcpkg install glfw3:x64-windows
```

### Clone Repository

```bash
git clone https://github.com/JanTopolewski/ball-trajectory-sim.git
cd ball-trajectory-sim
```

### Setup Dependencies

1. **Download GLAD** (OpenGL loader) from [https://glad.dav1d.de/](https://glad.dav1d.de/)
   - Settings: OpenGL 3.3+, Core Profile, Generate loader
   - Extract to `libs/glad/` (see [SETUP_DEPENDENCIES.md](SETUP_DEPENDENCIES.md))

2. **ImPlot** is already included (automatically downloaded)

3. **ImGui** is already included in the repository

### Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run
./build/TrajectorySimulation
```

## Project Structure

```
ball-trajectory-sim/
├── CMakeLists.txt           # Build configuration
├── src/                     # Source files
│   ├── main.cpp
│   ├── TrajectoryCalculator.cpp
│   └── FilesManager.cpp
├── include/                 # Header files
│   ├── TrajectoryCalculator.h
│   ├── FilesManager.h
│   ├── Simulation.h
│   ├── SpaceObject.h
│   └── Displaying.h
├── libs/                    # External libraries
│   ├── imgui/              ✓ Included
│   ├── implot/             ✓ Included
│   └── glad/               ⚠️ Download required
├── data/                    # Runtime data files
│   └── space_objects_data.csv
└── build/                   # Build directory (generated)
```

## Documentation

- **[QUICK_REFERENCE.md](QUICK_REFERENCE.md)** - Quick commands and tips
- **[CMAKE_TUTORIAL.md](CMAKE_TUTORIAL.md)** - Comprehensive CMake tutorial
- **[SETUP_DEPENDENCIES.md](SETUP_DEPENDENCIES.md)** - Detailed dependency setup guide

## Usage

1. **Launch the application**:
   ```bash
   ./build/TrajectorySimulation
   ```

2. **Create a new simulation**:
   - Set initial velocity, angle, mass, radius
   - Choose a space object preset (Earth, Moon, etc.) or custom values
   - Enable/disable gravity, wind, atmosphere
   - Set target distance

3. **View results**:
   - Watch the animated trajectory
   - Adjust parameters in real-time
   - Save simulations for later

4. **Load saved simulations**:
   - Choose "Read from file" at startup
   - Select a previously saved simulation

## Building on Different Platforms

### Linux
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/TrajectorySimulation
```

### Windows (Visual Studio)
```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
build\Release\TrajectorySimulation.exe
```

### Windows (MinGW)
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\TrajectorySimulation.exe
```

### macOS
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.ncpu)
./build/TrajectorySimulation
```

## Dependencies

### Required (System)
- **CMake** 3.15+
- **GLFW** 3.x - Window and input handling
- **OpenGL** 3.3+ - Graphics API
- **C++20** compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)

### Included (Vendored)
- **ImGui** - Immediate mode GUI library
- **ImPlot** - Plotting library for ImGui
- **GLAD** - OpenGL function loader (requires manual download)

## Troubleshooting

### "Could not find package glfw3"
Install GLFW3 development libraries:
```bash
sudo apt install libglfw3-dev  # Linux
brew install glfw              # macOS
vcpkg install glfw3:x64-windows  # Windows
```

### "glad/glad.h: No such file or directory"
Download GLAD from [https://glad.dav1d.de/](https://glad.dav1d.de/) and place in `libs/glad/`.
See [SETUP_DEPENDENCIES.md](SETUP_DEPENDENCIES.md) for details.

### "implot.h: No such file or directory"
ImPlot should already be included. If missing:
```bash
cd libs
git clone https://github.com/epezent/implot.git
```

For more troubleshooting, see [SETUP_DEPENDENCIES.md](SETUP_DEPENDENCIES.md).

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

See [LICENSE](LICENSE) file for details.

## Authors

- Original repository: [JanTopolewski/ball-trajectory-sim](https://github.com/JanTopolewski/ball-trajectory-sim)

## Acknowledgments

- [ImGui](https://github.com/ocornut/imgui) - Omar Cornut
- [ImPlot](https://github.com/epezent/implot) - Evan Pezent
- [GLFW](https://www.glfw.org/) - Marcus Geelnard, Camilla Löwy
- [GLAD](https://glad.dav1d.de/) - David Herberth
- [LearnOpenGL](https://learnopengl.com/) - Joey de Vries

## Instalacja ImPlot

Pobierz [ImPlot](https://github.com/epezent/implot) i wypakuj do z zipa

Dalej zrób tak samo jak z folderem ImGui:

- dodaj nowe filtry nazawne `implot` do `Header Files` i `Source Files` w projekcie TrajectorySimulation
- do filtru w `Header Files` dodaj rozpakowane pliki `.h` z folderu implot
- do filtru w `Source Files` dodaj rozpakowane pliki `.cpp` z folderu implot