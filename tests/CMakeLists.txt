cmake_minimum_required(VERSION 3.0)
project(etermal_test VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

add_executable(etermal_test EXCLUDE_FROM_ALL main.cpp)

file(COPY "lucon_aa.bmp" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

find_package(etermal 2.0 REQUIRED)
find_package(Freetype 2.1 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
target_link_libraries(etermal_test glfw)
target_link_libraries(etermal_test etermal)
target_link_libraries(etermal_test Freetype::Freetype)

add_custom_target(tests DEPENDS etermal_test)
