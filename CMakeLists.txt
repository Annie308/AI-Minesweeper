cmake_minimum_required(VERSION 3.15)
project(aIMinesweeper)

set(CMAKE_CXX_STANDARD 17)

# Vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:\\Users\\annie\\Documents\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

set(CMAKE_PREFIX_PATH "C:/Users/annie/Documents/vcpkg/installed/x64-windows")
# Find SFML using vcpkg-style CONFIG mode
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(aIMinesweeper src/main.cpp src/game.cpp src/render.cpp src/solver.cpp src/helpers.cpp)

# Include directories
target_include_directories(aIMinesweeper PRIVATE include)

target_link_libraries(aIMinesweeper PRIVATE SFML::Graphics)

