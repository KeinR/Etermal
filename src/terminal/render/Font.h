#ifndef ETERMAL_FONT_H_INCLUDED
#define ETERMAL_FONT_H_INCLUDED

#include <string>

#include "ftype.h"
#include "FontLibrary.h" // TEMP, do forward
#include "Texture.h"

namespace etm {
    class Font {
        FT_Face face;
        void free();
    public:
        Font(FontLibrary &lib, const std::string &path);
        // Font(Font &&other) = delete;
        ~Font();
        FT_Face getFace();
        // Font &operator=(Font &&other);
        void setSize(unsigned int size);
        // x/y are the top-left coordiates, with y increasing down.
        Texture renderChar(char c);
    };
}

#endif
