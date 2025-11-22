cmake_minimum_required(VERSION 3.10)
project(TrajectorySimulation)

set(CMAKE_CXX_STANDARD 17)

add_executable(TrajectorySimulation main.cpp "include/FilesManager.h" "src/FilesManager.cpp")

file(COPY ${PROJECT_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})