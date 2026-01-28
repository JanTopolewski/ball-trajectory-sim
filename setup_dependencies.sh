#!/bin/bash

# Ball Trajectory Simulation - Dependency Setup Script
# This script downloads and sets up GLAD and ImPlot for your project

set -e  # Exit on error

PROJECT_DIR="/home/wiktor/cpp/ball-trajectory-sim"
LIBS_DIR="$PROJECT_DIR/libs"

echo "=================================="
echo "Setting up project dependencies..."
echo "=================================="
echo ""

# Check if we're in the right directory
if [ ! -f "$PROJECT_DIR/CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found in $PROJECT_DIR"
    echo "Please run this script from the project directory or update PROJECT_DIR variable"
    exit 1
fi

cd "$PROJECT_DIR"

# ============================================================================
# 1. ImPlot - Plotting Library for ImGui
# ============================================================================
echo "[1/2] Setting up ImPlot..."

if [ -d "$LIBS_DIR/implot" ]; then
    echo "  ✓ ImPlot already exists at $LIBS_DIR/implot"
else
    echo "  Downloading ImPlot..."
    cd "$LIBS_DIR"
    git clone --depth 1 https://github.com/epezent/implot.git

    if [ -f "$LIBS_DIR/implot/implot.cpp" ]; then
        echo "  ✓ ImPlot downloaded successfully"
    else
        echo "  ✗ Failed to download ImPlot"
        exit 1
    fi
fi

echo ""

# ============================================================================
# 2. GLAD - OpenGL Function Loader
# ============================================================================
echo "[2/2] Setting up GLAD..."

if [ -d "$LIBS_DIR/glad" ] && [ -f "$LIBS_DIR/glad/src/glad.c" ]; then
    echo "  ✓ GLAD already exists at $LIBS_DIR/glad"
else
    echo "  ⚠️  GLAD requires manual download from https://glad.dav1d.de/"
    echo ""
    echo "  Please follow these steps:"
    echo "  1. Open https://glad.dav1d.de/ in your browser"
    echo "  2. Configure:"
    echo "     - Language: C/C++"
    echo "     - API > gl: Version 3.3 or higher"
    echo "     - Profile: Core"
    echo "     - Options: Check 'Generate a loader'"
    echo "  3. Click GENERATE"
    echo "  4. Download the ZIP file"
    echo ""
    echo "  Then extract it:"
    echo "     cd ~/Downloads"
    echo "     unzip glad.zip -d glad"
    echo "     mkdir -p $LIBS_DIR/glad"
    echo "     cp -r glad/include $LIBS_DIR/glad/"
    echo "     cp -r glad/src $LIBS_DIR/glad/"
    echo ""

    # Offer to open the URL
    if command -v xdg-open &> /dev/null; then
        read -p "  Would you like to open the GLAD website now? (y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            xdg-open "https://glad.dav1d.de/" &
            echo "  Browser opened. Please download GLAD and run this script again."
            exit 0
        fi
    fi

    echo "  ⚠️  Skipping GLAD setup - you'll need to download it manually"
fi

echo ""
echo "=================================="
echo "Dependency Setup Complete!"
echo "=================================="
echo ""

# ============================================================================
# Check what's installed
# ============================================================================
echo "Current status:"
echo "  ImGui:  ✓ Found"
[ -d "$LIBS_DIR/implot" ] && echo "  ImPlot: ✓ Found" || echo "  ImPlot: ✗ Missing"
[ -d "$LIBS_DIR/glad" ] && [ -f "$LIBS_DIR/glad/src/glad.c" ] && echo "  GLAD:   ✓ Found" || echo "  GLAD:   ✗ Missing"

echo ""

# ============================================================================
# Try to build
# ============================================================================
if [ -d "$LIBS_DIR/glad" ] && [ -d "$LIBS_DIR/implot" ]; then
    echo "All dependencies present! Would you like to build the project now?"
    read -p "Build now? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo ""
        echo "Building project..."
        echo "=================================="
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build -j$(nproc)

        if [ -f "build/TrajectorySimulation" ]; then
            echo ""
            echo "=================================="
            echo "✓ Build successful!"
            echo "=================================="
            echo ""
            echo "Run your simulation:"
            echo "  ./build/TrajectorySimulation"
            echo ""
        else
            echo ""
            echo "✗ Build failed. Check the errors above."
            exit 1
        fi
    fi
else
    echo ""
    echo "⚠️  Please download missing dependencies before building."
    echo "See SETUP_DEPENDENCIES.md for detailed instructions."
fi

echo ""
echo "For more information, see:"
echo "  - QUICK_REFERENCE.md      (Quick commands)"
echo "  - CMAKE_TUTORIAL.md       (Detailed tutorial)"
echo "  - SETUP_DEPENDENCIES.md   (Dependency setup)"
echo ""
