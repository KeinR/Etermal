#include "FontLibrary.h"

#include <string>
#include <stdexcept>

etm::FontLibrary::FontLibrary() {
    FT_Error error = FT_Init_FreeType(&lib);
    if (error != FT_Err_Ok) {
        lib = nullptr;
        throw std::logic_error(
            "etm::FontLibrary: Failed to create freetype library, code = " + std::to_string(error)
        );
    }
}
etm::FontLibrary::~FontLibrary() {
    if (lib != nullptr) {
        FT_Done_FreeType(lib);
    }
}
FT_Library etm::FontLibrary::get() {
    return lib;
}
