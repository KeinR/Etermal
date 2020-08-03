
set(ETERMAL_BUILD_INTERNAL_DOCS ${BUILD_PRIVATE_DOCS})

if (NOT ETERMAL_BUILD_INTERNAL_DOCS)
    message("-- Configured to public docs")
    set(ETERMAL_DOXYGEN_SRC
        "src/ETerminal.h"
        "src/TermInput.h"
        "src/EShell.h"
        "src/terminal/Terminal.h"
        "src/terminal/util/termError.h"
        "src/terminal/render/Font.h"
        "src/terminal/render/BmpFont.h"
        "src/terminal/render/EtmFont.h"
        "src/terminal/util/Enums.h"
        "src/shell/"
        "docs/"
    )
else()
    message("-- Configured to private docs")
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
