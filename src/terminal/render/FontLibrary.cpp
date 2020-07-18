#include "FontLibrary.h"

#include "../Resources.h"

etm::FontLibrary::FontLibrary(Resources *res) {
    FT_Error error = FT_Init_FreeType(&lib);
    if (error != FT_Err_Ok) {
        lib = nullptr;
        res->postError(
            "etm::FontLibrary::FontLibrary()",
            "Failed to create freetype library",
            error,
            true
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
