cmake_minimum_required(VERSION 3.0)
project(etermal VERSION 1.0)

# Configure compiler settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
if (NOT DEV)
    message("Configured to release build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -s -O2")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s -O2")
else()
    message("Configured to development build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")
endif()

# Obtain sources
file(GLOB_RECURSE sources src/*.cpp extern/*.c)
file(GLOB generalheaders include/*.h)

# Create the library
add_library(etermal STATIC ${sources})

# Find and add package dependencies
find_package(freetype 2.1 REQUIRED)
target_link_libraries(etermal Freetype::Freetype)

# installing

include(CMakePackageConfigHelpers)

set(GLFW_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/glfw3")

include(GNUInstallDirs)

install(
    TARGETS etermal
    EXPORT etermalConfig
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(
    FILES ${generalheaders}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/etermal"
)
# Include license too, otherwise I'd be violating my own terms :/
install(
    FILES LICENSE
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/etermal"
)
install(
    DIRECTORY "src/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/etermal/include"
    FILES_MATCHING PATTERN "*.h"
)
install(
    DIRECTORY "extern/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/etermal/extern"
    FILES_MATCHING PATTERN "*.h"
)
export(TARGETS etermal
    FILE "${CMAKE_CURRENT_BINARY_DIR}/etermalConfig.cmake"
)
install(EXPORT etermalConfig
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/etermal"
)

if (MAKE_DOCS)
    add_subdirectory(docs)
endif()

# packaging

set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "1.0")
set(CPACK_PACKAGE_VENDOR "KeinR")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Embeddable terminal and shell for OpenGL")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_MONOLITHIC_INSTALL true)

if (WIN32 OR CYGWIN)
  set(CPACK_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
endif()

include(CPack)