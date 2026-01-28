# Setup Guide for External Dependencies

This guide will help you set up all required dependencies for the Ball Trajectory Simulation project.

## Required Dependencies

### 1. GLAD (OpenGL Function Loader)

**What**: GLAD loads OpenGL function pointers so you can use modern OpenGL.

**Download**:
1. Go to https://glad.dav1d.de/
2. Configure:
   - **Language**: C/C++
   - **API > gl**: Version 3.3 (or higher, like 4.6)
   - **Profile**: Core
   - **Options**: Check "Generate a loader"
3. Click **GENERATE**
4. Download the `glad.zip` file

**Installation**:
```bash
# Extract the zip file
cd ~/Downloads
unzip glad.zip -d glad

# Copy to your project
mkdir -p ~/cpp/ball-trajectory-sim/libs/glad
cp -r glad/include ~/cpp/ball-trajectory-sim/libs/glad/
cp -r glad/src ~/cpp/ball-trajectory-sim/libs/glad/
```

**Expected structure**:
```
libs/glad/
├── include/
│   ├── glad/
│   │   └── glad.h
│   └── KHR/
│       └── khrplatform.h
└── src/
    └── glad.c
```

---

### 2. ImPlot (Plotting Library)

**What**: ImPlot provides plotting functionality for ImGui (used for trajectory visualization).

**Download**:
```bash
cd ~/cpp/ball-trajectory-sim/libs
git clone https://github.com/epezent/implot.git
```

**Alternative** (if you don't want git history):
```bash
cd ~/cpp/ball-trajectory-sim/libs
wget https://github.com/epezent/implot/archive/refs/heads/master.zip
unzip master.zip
mv implot-master implot
rm master.zip
```

**Expected structure**:
```
libs/implot/
├── implot.h
├── implot.cpp
├── implot_items.cpp
├── implot_internal.h
└── ...
```

---

### 3. GLFW (Window Management)

GLFW must be installed system-wide.

#### **Linux (Ubuntu/Debian)**
```bash
sudo apt update
sudo apt install libglfw3-dev
```

#### **Linux (Fedora/RHEL)**
```bash
sudo dnf install glfw-devel
```

#### **macOS**
```bash
brew install glfw
```

#### **Windows (vcpkg - Recommended)**
```bash
# Install vcpkg if you haven't
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# Install GLFW
./vcpkg install glfw3:x64-windows

# Tell CMake where to find it
./vcpkg integrate install
```

#### **Windows (Manual)**
1. Download from https://www.glfw.org/download.html
2. Extract to `C:\Libraries\glfw`
3. Add to CMake command:
   ```bash
   cmake -B build -DCMAKE_PREFIX_PATH="C:\Libraries\glfw"
   ```

---

### 4. OpenGL

OpenGL is usually already available on your system.

#### **Linux**
```bash
# Mesa OpenGL (most common)
sudo apt install libgl1-mesa-dev

# Or proprietary drivers (NVIDIA/AMD) usually include OpenGL
```

#### **macOS**
OpenGL comes pre-installed with Xcode Command Line Tools:
```bash
xcode-select --install
```

#### **Windows**
OpenGL comes with your graphics drivers (NVIDIA, AMD, Intel).

---

## Full Setup Script

### Linux (Ubuntu/Debian)
```bash
#!/bin/bash
cd ~/cpp/ball-trajectory-sim

# Install system dependencies
sudo apt update
sudo apt install -y \
    cmake \
    build-essential \
    libglfw3-dev \
    libgl1-mesa-dev \
    git \
    wget \
    unzip

# Download and setup GLAD
echo "Downloading GLAD..."
cd ~/Downloads
wget -O glad.zip "https://glad.dav1d.de/generated/tmpfx3kv7k_glad/glad.zip"
unzip glad.zip -d glad
mkdir -p ~/cpp/ball-trajectory-sim/libs/glad
cp -r glad/include ~/cpp/ball-trajectory-sim/libs/glad/
cp -r glad/src ~/cpp/ball-trajectory-sim/libs/glad/
rm -rf glad glad.zip

# Download ImPlot
echo "Downloading ImPlot..."
cd ~/cpp/ball-trajectory-sim/libs
git clone https://github.com/epezent/implot.git

echo "Setup complete!"
cd ~/cpp/ball-trajectory-sim
```

### macOS
```bash
#!/bin/bash
cd ~/cpp/ball-trajectory-sim

# Install Homebrew if not installed
if ! command -v brew &> /dev/null; then
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install dependencies
brew install cmake glfw

# Download ImPlot
cd libs
git clone https://github.com/epezent/implot.git

# GLAD - must be downloaded manually from https://glad.dav1d.de/
echo "Please download GLAD from https://glad.dav1d.de/ and place in libs/glad/"

cd ..
```

---

## Building the Project

### Step 1: Configure
```bash
cd ~/cpp/ball-trajectory-sim
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### Step 2: Build
```bash
cmake --build build -j$(nproc)  # Linux/macOS
cmake --build build              # Windows
```

### Step 3: Run
```bash
./build/TrajectorySimulation     # Linux/macOS
build\Release\TrajectorySimulation.exe  # Windows
```

---

## Troubleshooting

### "Could not find GLFW"
- **Linux**: `sudo apt install libglfw3-dev`
- **macOS**: `brew install glfw`
- **Windows**: Use vcpkg or set `CMAKE_PREFIX_PATH`

### "glad/glad.h: No such file or directory"
Download GLAD from https://glad.dav1d.de/ and place in `libs/glad/`

### "implot.h: No such file or directory"
```bash
cd libs
git clone https://github.com/epezent/implot.git
```

### "cannot find -lGL" (Linux)
```bash
sudo apt install libgl1-mesa-dev
```

### Windows: "LINK: fatal error LNK1104: cannot open file 'glfw3.lib'"
Install GLFW via vcpkg:
```bash
vcpkg install glfw3:x64-windows
vcpkg integrate install
```

---

## Verifying Your Setup

Check that you have the following structure:

```
ball-trajectory-sim/
├── CMakeLists.txt ✓
├── libs/
│   ├── imgui/ ✓
│   │   ├── imgui.cpp
│   │   ├── imgui.h
│   │   └── ...
│   ├── glad/ ← DOWNLOAD THIS
│   │   ├── include/
│   │   │   ├── glad/glad.h
│   │   │   └── KHR/khrplatform.h
│   │   └── src/glad.c
│   └── implot/ ← DOWNLOAD THIS
│       ├── implot.cpp
│       ├── implot.h
│       └── ...
├── src/
│   ├── main.cpp ✓
│   └── ...
└── include/ ✓
```

System packages installed:
- ✓ GLFW3 (`find_package(glfw3)` should work)
- ✓ OpenGL (`find_package(OpenGL)` should work)

---

## Quick Start (Copy-Paste for Linux)

```bash
# Navigate to project
cd ~/cpp/ball-trajectory-sim

# Install system dependencies
sudo apt install -y cmake build-essential libglfw3-dev libgl1-mesa-dev git

# Download ImPlot
cd libs
git clone https://github.com/epezent/implot.git
cd ..

# Download GLAD manually from https://glad.dav1d.de/
# (You must do this step manually - select gl 3.3+, Core profile, Generate loader)
# Then extract to libs/glad/

# Build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Run
./build/TrajectorySimulation
```

---

**Need Help?** Check the [CMAKE_TUTORIAL.md](CMAKE_TUTORIAL.md) for detailed explanations.
