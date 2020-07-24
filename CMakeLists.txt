cmake_minimum_required(VERSION 3.0) # setting this is required
project(Etermal VERSION 1.0)

# Gather source files
file(GLOB_RECURSE sources src/*.cpp extern/*.c)

# Configure compiler settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -g")
set(CMAKE_C_FLAGS "-g -Wall")

# Create the executable
add_executable(Etermal ${sources})

# Find package dependencies
find_package(glfw3 3.3 REQUIRED)
find_package(freetype 2.1 REQUIRED)

# Add include directories
target_include_directories(Etermal PUBLIC extern)

target_link_libraries(Etermal glfw)
target_link_libraries(Etermal Freetype::Freetype)
