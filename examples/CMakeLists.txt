cmake_minimum_required(VERSION 3.0)
project(basic_example VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

add_executable(basic_example EXCLUDE_FROM_ALL basic.cpp ../extern/glad/glad.c)

find_package(etermal 2.0 REQUIRED)
find_package(freetype 2.1 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(basic_example etermal)
target_link_libraries(basic_example Freetype::Freetype)
target_link_libraries(basic_example glfw)

add_custom_target(examples DEPENDS basic_example)
