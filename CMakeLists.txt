cmake_minimum_required(VERSION 4.0)
project(TrajectorySimulation LANGUAGES C CXX)

# ============================================================================
# Project Configuration
# ============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# OpenGL
find_package(OpenGL REQUIRED)

# ============================================================================
# GLAD - OpenGL Function Loader
# ============================================================================
# GLAD needs to be downloaded from: https://glad.dav1d.de/
# Settings: gl Version 3.3+, Core profile, Generate a loader
# Place generated files in libs/glad/

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/external/include/glad/glad.c")
    message(WARNING "GLAD not found in external/include/glad/glad.c. Please download from https://glad.dav1d.de/")
    message(WARNING "Settings: OpenGL Version 3.3+, Core Profile, Generate a loader")
endif()

add_library(glad external/include/glad/glad.c)
target_include_directories(glad PUBLIC
        external/include/glad
)

# ============================================================================
# Dear ImGui - Immediate Mode GUI Library
# ============================================================================
# ImGui is vendored in libs/imgui/

add_library(imgui STATIC
    libs/imgui/imgui.cpp
    libs/imgui/imgui_draw.cpp
    libs/imgui/imgui_tables.cpp
    libs/imgui/imgui_widgets.cpp
    libs/imgui/imgui_demo.cpp
    # ImGui backends for GLFW and OpenGL3
    libs/imgui/imgui_impl_glfw.cpp
    libs/imgui/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/libs/imgui
    ${CMAKE_SOURCE_DIR}/libs/glad/include
)

# Fix for ImGui internal includes that use "../" relative paths
# The ImGui source files use "#include ../imgui.h" so we need to add a parent-like path
target_include_directories(imgui PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/imgui/..
    ${CMAKE_SOURCE_DIR}/libs
)

target_link_libraries(imgui PUBLIC
    glad
    glfw
    OpenGL::GL
)

# Tell ImGui to use a custom OpenGL loader (we're using GLAD)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_CUSTOM)

# ============================================================================
# ImPlot - Plotting Library for ImGui
# ============================================================================
# ImPlot needs to be downloaded from: https://github.com/epezent/implot
# Clone or download and place in libs/implot/

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/libs/implot/implot.cpp")
    message(WARNING "ImPlot not found in libs/implot/. Please download from:")
    message(WARNING "  https://github.com/epezent/implot")
    message(WARNING "Place implot.h, implot.cpp, implot_items.cpp, implot_internal.h in libs/implot/")
else()
    add_library(implot STATIC
        libs/implot/implot.cpp
        libs/implot/implot_items.cpp
    )

    target_include_directories(implot PUBLIC
        libs/implot
    )

    target_link_libraries(implot PUBLIC
        imgui
    )
endif()

# ============================================================================
# Main Executable - Your Application
# ============================================================================

add_executable(TrajectorySimulation
    # Main entry point
    src/main.cpp

    # Your application source files
    src/TrajectoryCalculator.cpp
    src/FilesManager.cpp

    # Header files (optional but helps IDEs)
    include/TrajectoryCalculator.h
    include/FilesManager.h
    include/Displaying.h
    include/Simulation.h
    include/SpaceObject.h
)

# Include directories for your headers
target_include_directories(TrajectorySimulation PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link all dependencies
target_link_libraries(TrajectorySimulation PRIVATE
    imgui
    glad
    glfw
    OpenGL::GL
)

# Link implot if available
if(TARGET implot)
    target_link_libraries(TrajectorySimulation PRIVATE implot)
endif()

# ============================================================================
# Platform-Specific Settings
# ============================================================================

if(WIN32)
    # Windows: Link additional system libraries
    target_link_libraries(TrajectorySimulation PRIVATE
        gdi32
        shell32
        user32
    )

    # Optional: Set as GUI application (no console window)
    # set_target_properties(TrajectorySimulation PROPERTIES WIN32_EXECUTABLE ON)

elseif(APPLE)
    # macOS: Link required frameworks
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)

    target_link_libraries(TrajectorySimulation PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )

elseif(UNIX)
    # Linux: Link system libraries
    target_link_libraries(TrajectorySimulation PRIVATE
        dl          # Dynamic loading
        pthread     # Threading
    )

    # Optional: Link X11 if needed
    # find_package(X11 REQUIRED)
    # target_link_libraries(TrajectorySimulation PRIVATE ${X11_LIBRARIES})
endif()

# ============================================================================
# Copy Runtime Resources
# ============================================================================

# Copy data directory to build directory
file(COPY ${CMAKE_SOURCE_DIR}/data
     DESTINATION ${CMAKE_BINARY_DIR})

# Alternative: Use custom command to copy on each build
# add_custom_command(TARGET TrajectorySimulation POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#     ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
# )

# ============================================================================
# Build Type Settings
# ============================================================================

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "")
message(STATUS "==================== Build Configuration ====================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "=============================================================")
message(STATUS "")
