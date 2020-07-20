#ifndef ETERMAL_FONT_H_INCLUDED
#define ETERMAL_FONT_H_INCLUDED

#include <string>
#include <map>

#include "ftype.h"
#include "Texture.h"

namespace etm {
    // ../Resources
    class Resources;
    // FontLibrary
    class FontLibrary;
}

namespace etm {
    class Font {
    public:
        typedef unsigned int char_t;
    private:
        typedef std::map<char_t, Texture> textCache_t;

        Resources *res;
        FT_Face face;
        int charWidth;
        int charHeight;

        textCache_t textCache;

        void free();
        void calcCharSize();
        // Generate a new texture for the char.
        // Way slow, hence use of cache
        Texture makeCharTexture(char_t c);
    public:
        Font(Resources *res, FontLibrary &lib, const std::string &path);
        Font(Font &&other) = delete; // TEMP
        ~Font();
        // Font &operator=(Font &&other);
        // The pixel size of the font
        void setSize(unsigned int size);
        // Binds char texture to active texture slot
        void bindChar(char_t c);
        // Clear the character cache
        void clearCache();
        // The width and height of each char
        int getCharWidth();
        int getCharHeight();
    };
}

#endif
