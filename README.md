# Ball trajectory simulation

A cross-platform C++ application for simulating and visualizing ball trajectories with physics calculations, featuring a graphical interface built with ImGui and ImPlot.

![C++](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20-lightgrey.svg)

## Dependencies
- **CMake** 3.15+
- **OpenGL** 3.3+ - Graphics API
- **C++20** compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- **ImGui** - Immediate mode GUI library
- **ImPlot** - Plotting library for ImGui
- **GLAD** - OpenGL function loader (requires manual download)
- **GLFW** 3.x - Window and input handling

## Setup

### Prerequisites

**You have to have certain system dependencies installed on your computer:**
- OpenGL in version 3.3 or higher
- CMake
- git version control system

**Installation**

<details>
<summary>Linux</summary>
On Linux you can install them by running this command:

```bash
# Ubuntu/Debian
sudo apt install cmake build-essential libgl1-mesa-dev git
```
</details>

<details>
<summary>Windows</summary>
On Windows you need to manually install everything from a web browser

1. First you should check if you already have OpenGL in 3.3 version, you can do it by installing and running an OpenGL checker program like [GLView](https://www.realtech-vr.com/glview/)
2. Then you need to have [CMake](https://cmake.org/download/) installed for windows
3. Git version control system is needed only to clone the repo, if you don't already have git installed, you can just download the .zip archive of code and extract it
</details>

### Clone Repository

```bash
git clone https://github.com/JanTopolewski/ball-trajectory-sim.git
cd ball-trajectory-sim
```

### Setup Dependencies

1. **GLAD** (OpenGL loader) 
   - Download it from [https://glad.dav1d.de/](https://glad.dav1d.de/)
   - Settings: OpenGL 3.3+, Core Profile, Generate loader
   - Extract to `external/include/glad/` 

2. **GLFW**
   <details>
   <summary>Linux</summary>
   
   </details>
   
   <details>
   <summary>Windows</summary>
   
   1. download the source package of [GLFW](https://www.glfw.org/download.html)
   2. extract the glfw .zip file and open cmake gui
   3. as the source code, select the just extracted folder
   4. create the 'build' subfolder and select it as the build folder
   5. click on configure and make sure you have 'Visual Studio ...' option selected as the generator
   6. exit the modal window and click on 'configure'
   7. click on 'generate' and exit from cmake
   8. open the extracted glfw folder, go to ./build subfolder and open the generated .sln or .slnx file in Visual Studio
   9. right click on solution 'GLFW' in solution explorer and choose 'Build Solution'
   10. once the build is finished exit from Visual Studio
   11. go to your extracted glfw folder then ./build/src/Debug 
   12. copy-paste the glfw3.lib file to this project's ./external/libs/ folder
   13. go back to the extracted glfw folder
   14. go to ./include subfolder and copy-paste the ./GLFW folder into this project's ./external/include folder
   </details>

3. **ImPlot** 
   - Download the code in .zip archive from the [github repo](https://github.com/epezent/implot)
   - Extract the zip to `external/include/implot`

4. **ImGui**
   - Download the code in .zip archive from the [github repo](https://github.com/ocornut/imgui)
   - Extract the zip to `external/include/imgui`

### Build

<details>
<summary>Linux</summary>

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/TrajectorySimulation
```

</details>

<details>
<summary>Windows</summary>

#### Visual Studio

```bash
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
build\Release\TrajectorySimulation.exe
```

#### MinGW
```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\TrajectorySimulation.exe
```
</details>

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
├── external/                # External libraries
│   ├── include/             # .h and .cpp files
│   │   ├── glad/
│   │   │   ├── glad/
│   │   │   ├── KHR/
│   │   │   └── glad.c
│   │   ├── GLFW/
│   │   ├── imgui/
│   │   └── implot/       
│   └── libs/                # compiled static libraries
│       ├── libglfw3.a       # for Linux
│       └── glfw3.lib        # for Windows      
├── data/                    # Runtime data files
│   └── space_objects_data.csv
└── build/                   # Build directory
```

## Usage

1. **Create a new simulation**:
   - Set initial velocity, angle, mass, radius
   - Choose a space object preset (Earth, Moon, etc.) or custom values
   - Enable/disable gravity, wind, atmosphere
   - Set target distance

2. **View results**:
   - Watch the animated trajectory
   - Adjust parameters in real-time
   - Save simulations for later

3. **Load saved simulations**:
   - Choose "Read from file" at startup
   - Select a previously saved simulation

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
