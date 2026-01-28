# CMake Quick Reference for Your Project

## 🚀 Quick Build Commands

### Clean Build
```bash
cd ~/cpp/ball-trajectory-sim
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/TrajectorySimulation
```

### Rebuild After Code Changes
```bash
cmake --build build
./build/TrajectorySimulation
```

### Debug Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
gdb ./build/TrajectorySimulation
```

---

## 📁 Project Structure

```
ball-trajectory-sim/
├── CMakeLists.txt          # Build configuration (WHAT YOU EDITED)
├── CMAKE_TUTORIAL.md       # Detailed tutorial
├── SETUP_DEPENDENCIES.md   # How to install dependencies
├── 
├── src/                    # Your C++ source files
│   ├── main.cpp
│   ├── TrajectoryCalculator.cpp
│   └── FilesManager.cpp
│
├── include/                # Your header files
│   ├── TrajectoryCalculator.h
│   ├── FilesManager.h
│   ├── Displaying.h
│   ├── Simulation.h
│   └── SpaceObject.h
│
├── libs/                   # External libraries (vendored)
│   ├── imgui/             ✓ Already present
│   ├── glad/              ⚠️ NEED TO DOWNLOAD
│   └── implot/            ⚠️ NEED TO DOWNLOAD
│
├── data/                   # Runtime data files
│   └── space_objects_data.csv
│
└── build/                  # Generated files (git ignored)
    └── TrajectorySimulation
```

---

## 🔧 What the CMakeLists.txt Does

### 1. **Sets Up the Project**
```cmake
cmake_minimum_required(VERSION 3.15)
project(TrajectorySimulation VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
```
- Minimum CMake version needed
- Project name and version
- C++20 standard

### 2. **Finds System Libraries**
```cmake
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
```
- OpenGL: Graphics API
- GLFW: Window and input handling

### 3. **Builds GLAD Library**
```cmake
add_library(glad STATIC libs/glad/src/glad.c)
target_include_directories(glad PUBLIC libs/glad/include)
```
- Creates static library from GLAD source
- Makes headers available

### 4. **Builds ImGui Library**
```cmake
add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    # ... more files
)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)
```
- Compiles all ImGui source files
- Links against GLFW and OpenGL

### 5. **Builds ImPlot Library**
```cmake
add_library(implot STATIC
    libs/implot/implot.cpp
    libs/implot/implot_items.cpp
)
target_link_libraries(implot PUBLIC imgui)
```
- Compiles ImPlot (if present)
- Links against ImGui

### 6. **Builds Your Application**
```cmake
add_executable(TrajectorySimulation
    src/main.cpp
    src/TrajectoryCalculator.cpp
    src/FilesManager.cpp
)
target_include_directories(TrajectorySimulation PRIVATE include)
target_link_libraries(TrajectorySimulation PRIVATE imgui glad glfw OpenGL::GL implot)
```
- Compiles your source files
- Includes your header directory
- Links all dependencies

### 7. **Platform-Specific Settings**
```cmake
if(WIN32)
    # Windows libraries
elseif(APPLE)
    # macOS frameworks
elseif(UNIX)
    # Linux libraries (dl, pthread)
endif()
```
- Handles differences between Windows, macOS, and Linux

### 8. **Copies Data Files**
```cmake
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
```
- Copies `data/` folder to `build/` so your executable can find it

---

## 🎯 Key CMake Concepts

### Targets
A "target" is something you build (executable or library):
- `glad` - Library target
- `imgui` - Library target
- `implot` - Library target
- `TrajectorySimulation` - Executable target

### Linking
When you link target A to target B, A can use B's code:
```cmake
target_link_libraries(TrajectorySimulation PRIVATE imgui)
```
Now `TrajectorySimulation` can use ImGui functions.

### Include Directories
Where the compiler looks for `.h` files:
```cmake
target_include_directories(TrajectorySimulation PRIVATE include)
```
Now you can `#include "TrajectoryCalculator.h"` from `src/main.cpp`.

### PUBLIC vs PRIVATE
- **PUBLIC**: Available to this target AND things that link to it
- **PRIVATE**: Only available to this target
- **INTERFACE**: Only available to things that link to it

Example:
```cmake
target_link_libraries(imgui PUBLIC glfw)  # Things using imgui also get glfw
target_link_libraries(TrajectorySimulation PRIVATE imgui)  # Only TrajectorySimulation uses imgui
```

---

## ⚙️ Build Types

### Release (Optimized, fast)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```
- Full optimizations (-O3)
- No debug symbols
- Use for final executable

### Debug (Debugging info)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```
- No optimizations (-O0)
- Debug symbols included (-g)
- Use with GDB/debugger

### RelWithDebInfo (Both)
```bash
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
```
- Optimizations + debug symbols
- Good for profiling

---

## 🐛 Common Issues & Solutions

### Issue: "Could not find package glfw3"
**Solution**:
```bash
# Linux
sudo apt install libglfw3-dev

# macOS
brew install glfw

# Windows
vcpkg install glfw3:x64-windows
```

### Issue: "glad/glad.h: No such file or directory"
**Solution**: Download GLAD from https://glad.dav1d.de/
- Settings: OpenGL 3.3+, Core profile, Generate loader
- Extract to `libs/glad/`

### Issue: "implot.h: No such file or directory"
**Solution**:
```bash
cd libs
git clone https://github.com/epezent/implot.git
```

### Issue: Linker errors (undefined reference)
**Possible causes**:
1. Missing source file in `add_executable()`
2. Missing library in `target_link_libraries()`
3. Wrong link order (rare)

**Solution**: Check CMakeLists.txt for missing files/libraries

### Issue: "cannot find -lGL" (Linux)
**Solution**:
```bash
sudo apt install libgl1-mesa-dev
```

---

## 🔍 Debugging CMake

### See all CMake variables
```bash
cmake -B build -LAH
```

### See what libraries are found
```bash
cmake -B build 2>&1 | grep -i "found"
```

### See compilation commands
```bash
cmake -B build -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build
```

### Clean and reconfigure
```bash
rm -rf build
cmake -B build
```

---

## 📝 Modifying CMakeLists.txt

### Add a new source file
```cmake
add_executable(TrajectorySimulation
    src/main.cpp
    src/TrajectoryCalculator.cpp
    src/FilesManager.cpp
    src/YourNewFile.cpp  # ADD THIS
)
```

### Add a new header directory
```cmake
target_include_directories(TrajectorySimulation PRIVATE
    include
    external/somelib/include  # ADD THIS
)
```

### Add a new library
```cmake
# Find it
find_package(SomeLib REQUIRED)

# Link it
target_link_libraries(TrajectorySimulation PRIVATE SomeLib::SomeLib)
```

---

## 🌍 Cross-Platform Tips

### Paths
✅ Use `/` (forward slashes) - CMake converts automatically
❌ Don't hardcode `\` (backslashes) - Windows-only

### File Operations
✅ Use `${CMAKE_SOURCE_DIR}/data`
❌ Don't use `~/` or `C:\` - not portable

### Compilers
- **Linux**: GCC or Clang
- **macOS**: Clang (from Xcode)
- **Windows**: MSVC, MinGW, or Clang

CMake handles compiler differences automatically!

---

## 📚 Next Steps

1. ✅ Read [CMAKE_TUTORIAL.md](CMAKE_TUTORIAL.md) for deep dive
2. ⚠️ Follow [SETUP_DEPENDENCIES.md](SETUP_DEPENDENCIES.md) to install missing libraries
3. 🏗️ Build your project:
   ```bash
   cmake -B build
   cmake --build build
   ./build/TrajectorySimulation
   ```

---

## 🎓 Learning Resources

- **Official Docs**: https://cmake.org/documentation/
- **Modern CMake**: https://cliutils.gitlab.io/modern-cmake/
- **CMake Examples**: https://github.com/ttroy50/cmake-examples
- **Stack Overflow**: Search "cmake [your problem]"

---

## ✅ Checklist Before Building

- [ ] CMakeLists.txt exists and is configured
- [ ] System dependencies installed (GLFW, OpenGL)
- [ ] GLAD downloaded and placed in `libs/glad/`
- [ ] ImPlot downloaded and placed in `libs/implot/`
- [ ] All `.cpp` files listed in `add_executable()`
- [ ] All `.h` paths added to `target_include_directories()`

**Ready to build?**
```bash
cmake -B build && cmake --build build && ./build/TrajectorySimulation
```

Good luck! 🚀
