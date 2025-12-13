cmake_minimum_required(VERSION 3.10)
project(TrajectorySimulation)

set(CMAKE_CXX_STANDARD 20)

add_executable(TrajectorySimulation "${PROJECT_SOURCE_DIR}/src/main.cpp" "include/TrajectoryCalculator.h" "src/TrajectoryCalculator.cpp" "include/FilesManager.h" "src/FilesManager.cpp" "include/Displaying.h" "include/Simulation.h" "include/SpaceObject.h")
file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
