# CMake Tutorial for Ball Trajectory Simulation

## What is CMake?

CMake is a **cross-platform build system generator**. It doesn't build your code directly—instead, it generates native build files (Makefiles on Linux, Visual Studio projects on Windows, Xcode projects on macOS) that your compiler then uses.

### Key Concepts

1. **CMakeLists.txt**: The configuration file that tells CMake how to build your project
2. **Targets**: Things you want to build (executables, libraries)
3. **Dependencies**: External libraries your project needs (GLFW, ImGui, etc.)
4. **Out-of-source builds**: Building in a separate directory (e.g., `build/`) keeps your source clean

---

## How CMake Works

### Basic Workflow

```
CMakeLists.txt → CMake → Build System (Makefile/VS Project) → Compiler → Executable
```

### Step-by-Step Process

1. **Configure**: CMake reads CMakeLists.txt and generates build files
   ```bash
   cmake -B build          # Linux/Mac
   cmake -B build -G "Visual Studio 17 2022"  # Windows
   ```

2. **Build**: The actual compilation happens
   ```bash
   cmake --build build
   ```

3. **Run**: Execute your program
   ```bash
   ./build/TrajectorySimulation  # Linux/Mac
   build\Debug\TrajectorySimulation.exe  # Windows
   ```

---

## Understanding Your CMakeLists.txt

### 1. Basic Project Setup

```cmake
cmake_minimum_required(VERSION 3.15)
project(TrajectorySimulation VERSION 1.0)
```

- **cmake_minimum_required**: Ensures users have a compatible CMake version
- **project()**: Names your project and sets variables like `PROJECT_NAME`, `PROJECT_SOURCE_DIR`

### 2. C++ Standard

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

- Sets C++20 as your standard
- `REQUIRED` ensures it fails if the compiler doesn't support C++20

### 3. Finding System Dependencies

```cmake
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
```

- **find_package**: Searches for installed libraries
- CMake looks in standard locations (`/usr/lib`, `C:\Program Files`, etc.)
- `REQUIRED` means build fails if not found

### 4. Adding Vendored Libraries (ImGui)

For libraries you include in your source tree (like `libs/imgui`):

```cmake
add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_impl_glfw.cpp
    libs/imgui/imgui_impl_opengl3.cpp
)
```

- **add_library**: Creates a library target
- **STATIC**: Builds a static library (.a on Linux, .lib on Windows)
- Lists all `.cpp` files needed

### 5. Linking Libraries

```cmake
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
```

- **target_link_libraries**: Links dependencies to your target
- **PUBLIC**: Dependencies propagate to anything that links against imgui
- **PRIVATE**: Only this target needs the dependency
- **INTERFACE**: Only targets linking against this need the dependency

### 6. Include Directories

```cmake
target_include_directories(imgui PUBLIC libs/imgui)
```

- Tells the compiler where to find header files
- **PUBLIC**: Other targets linking against this can also use these headers

### 7. Creating Your Executable

```cmake
add_executable(TrajectorySimulation
    src/main.cpp
    src/TrajectoryCalculator.cpp
    src/FilesManager.cpp
)

target_include_directories(TrajectorySimulation PRIVATE include)
target_link_libraries(TrajectorySimulation PRIVATE imgui glad)
```

- **add_executable**: Creates your main program
- **PRIVATE**: Include directories only for this target
- Links all dependencies

---

## Cross-Platform Considerations

### Platform-Specific Code

```cmake
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(TrajectorySimulation PRIVATE WINDOWS_BUILD)
elseif(APPLE)
    # macOS-specific settings
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    target_link_libraries(TrajectorySimulation PRIVATE ${COCOA_LIBRARY})
elseif(UNIX)
    # Linux-specific settings
    target_link_libraries(TrajectorySimulation PRIVATE dl pthread)
endif()
```

### Common Cross-Platform Issues

1. **Path Separators**: Use `/` or `${CMAKE_SOURCE_DIR}` - CMake handles conversion
2. **Library Names**: 
   - Linux: `libglfw3.a` → `-lglfw3`
   - Windows: `glfw3.lib`
   - CMake abstracts this with `glfw`
3. **OpenGL**: Different per platform
   - Linux: `-lGL`
   - Windows: `opengl32.lib`
   - macOS: `-framework OpenGL`
   - CMake handles via `OpenGL::GL`

---

## External Dependencies Management

### Option 1: System-Installed (Recommended for GLFW, OpenGL)

**Linux (Ubuntu/Debian):**
```bash
sudo apt install libglfw3-dev libgl1-mesa-dev
```

**macOS:**
```bash
brew install glfw
```

**Windows:**
- Use vcpkg: `vcpkg install glfw3:x64-windows`
- Or download pre-built binaries and set `CMAKE_PREFIX_PATH`

### Option 2: Vendored in Your Project (Your ImGui approach)

Place libraries in `libs/` and use `add_library()` or `add_subdirectory()`.

**Pros:**
- Version control
- No external installation needed
- Consistent across platforms

**Cons:**
- Larger repository
- Manual updates

### Option 3: FetchContent (Modern CMake)

```cmake
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)
```

CMake automatically downloads and builds dependencies.

---

## Building Your Project

### Linux/macOS

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run
./build/TrajectorySimulation
```

### Windows (Visual Studio)

```bash
# Configure
cmake -B build -G "Visual Studio 17 2022"

# Build
cmake --build build --config Release

# Run
build\Release\TrajectorySimulation.exe
```

### Windows (MinGW)

```bash
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\TrajectorySimulation.exe
```

---

## Important CMake Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `PROJECT_NAME` | Your project name | TrajectorySimulation |
| `PROJECT_SOURCE_DIR` | Root directory | /home/user/project |
| `CMAKE_BINARY_DIR` | Build directory | /home/user/project/build |
| `CMAKE_CXX_STANDARD` | C++ version | 20 |
| `CMAKE_BUILD_TYPE` | Debug/Release | Release |
| `CMAKE_PREFIX_PATH` | Where to find packages | /usr/local;C:\libs |

---

## Debugging CMake Issues

### Common Problems

1. **"Could not find package X"**
   - Install the package system-wide
   - Set `CMAKE_PREFIX_PATH` to where it's installed
   - Use `find_package(X REQUIRED)` and check error messages

2. **Linker errors (undefined reference)**
   - Missing `target_link_libraries()` call
   - Wrong library order (some platforms care)
   - Missing source files in `add_executable()`

3. **Header not found**
   - Missing `target_include_directories()`
   - Wrong path in `#include`

### Useful Debug Commands

```cmake
message(STATUS "OpenGL found: ${OPENGL_FOUND}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
```

---

## Best Practices

1. ✅ **Use modern CMake** (version 3.15+)
2. ✅ **Use targets, not variables** (`OpenGL::GL` not `${OPENGL_LIBRARIES}`)
3. ✅ **Out-of-source builds** (build in `build/`, not source directory)
4. ✅ **Specify PUBLIC/PRIVATE/INTERFACE** for clarity
5. ✅ **Use `target_*` commands** instead of global ones
6. ❌ **Avoid `include_directories()`** (global) - use `target_include_directories()`
7. ❌ **Avoid `link_libraries()`** (global) - use `target_link_libraries()`

---

## Your Project Structure Explained

```
ball-trajectory-sim/
├── CMakeLists.txt          # Main build configuration
├── include/                # Your header files (.h)
│   ├── TrajectoryCalculator.h
│   ├── FilesManager.h
│   └── ...
├── src/                    # Your source files (.cpp)
│   ├── main.cpp
│   ├── TrajectoryCalculator.cpp
│   └── FilesManager.cpp
├── libs/                   # Vendored external libraries
│   └── imgui/
│       ├── imgui.h
│       ├── imgui.cpp
│       └── ...
├── data/                   # Runtime data files
│   └── space_objects_data.csv
└── build/                  # Generated build files (not in git)
    └── TrajectorySimulation
```

---

## Additional Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [Modern CMake Tutorial](https://cliutils.gitlab.io/modern-cmake/)
- [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1)
- [CMake Examples](https://github.com/ttroy50/cmake-examples)

---

## Next Steps

1. Install required system dependencies (GLFW, OpenGL)
2. Download GLAD and ImPlot if not already present
3. Run `cmake -B build`
4. Run `cmake --build build`
5. Execute `./build/TrajectorySimulation`

Good luck with your trajectory simulation! 🚀
