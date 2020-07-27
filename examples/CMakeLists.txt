cmake_minimum_required(VERSION 3.1) 
project(examples VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(examples basic.cpp ../extern/glad/glad.c)

find_package(glfw3 3.3 REQUIRED)
find_package(freetype 2.1 REQUIRED)
find_package(etermal REQUIRED)

target_link_libraries(examples Freetype::Freetype)
target_link_libraries(examples glfw)
target_link_libraries(examples etermal)
