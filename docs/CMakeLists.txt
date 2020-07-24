
if (NOT BUILD_PRIVATE_DOCS)
    set(ETERMAL_DOXYGEN_SRC
        "src/ETerminal.h"
        "src/TermInput.h"
        "src/EShell.h"
        "src/terminal/Terminal.h"
        "src/shell/"
        "docs/"
    )
else()
    set(ETERMAL_DOXYGEN_SRC
        "src/"
        "docs/"
    )
endif()

set(DOXYGEN_SKIP_DOT true)
find_package(Doxygen REQUIRED)

foreach(path IN LISTS ETERMAL_DOXYGEN_SRC)
    set(ETERMAL_DOXYGEN_INPUT "${ETERMAL_DOXYGEN_INPUT} \\\n\"${CMAKE_SOURCE_DIR}/${path}\"")
endforeach()

configure_file(Doxyfile.in Doxyfile @ONLY)

add_custom_target(docs "${DOXYGEN_EXECUTABLE}")
