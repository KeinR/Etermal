#ifndef ETERMAL_FONT_H_INCLUDED
#define ETERMAL_FONT_H_INCLUDED

#include <string>

#include "ftype.h"
#include "FontLibrary.h" // TEMP, do forward
#include "Texture.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {
    class Font {
        Resources *res;
        FT_Face face;
        int charWidth;
        int charHeight;
        void free();
        void calcCharSize();
    public:
        Font(Resources *res, FontLibrary &lib, const std::string &path);
        // Font(Font &&other) = delete;
        ~Font();
        // FT_Face getFace();
        // Font &operator=(Font &&other);
        void setSize(unsigned int size);
        // x/y are the top-left coordiates, with y increasing down.
        Texture renderChar(char c);
        int getCharWidth();
        int getCharHeight();
    };
}

#endif
