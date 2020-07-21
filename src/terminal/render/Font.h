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

    /**
    * Takes care of rendering codepoints and binding their
    * textures.
    * Works with a cache to save run time.
    */
    class Font {
    public:
        /// Codepoint type
        typedef unsigned int char_t;
    private:
        /// The text cache type
        typedef std::map<char_t, Texture> textCache_t;

        /// Handle to the @ref Resources object
        Resources *res;
        /// Handle to the Freetype face
        /// (used for rendering)
        /// @see makeCharTexture(char_t c)
        FT_Face face;
        /// Width of each char
        int charWidth;
        /// Height of each char
        int charHeight;

        /// Cache of all the generated char textures
        /// @see makeCharTexture(char_t c)
        /// @see bindChar(char_t c)
        /// @see clearCache()
        textCache_t textCache;

        /**
        * Releases dynamic resources
        */
        void free();
        /**
        * Recalculate the @ref charWidth and @ref charHeight.
        */
        void calcCharSize();
        /**
        * Generate a texture for the given char.
        * Really slow, hence the use of a cache
        * @see textCache
        * @see bindChar(char_t c)
        */
        Texture makeCharTexture(char_t c);
    public:
        /**
        * Construct a Font.
        * @warning `lib` must destruct after `*this`, because @ref face is generated
        * from it.
        * @param [in] res A @ref Resources object
        * @param [in,out] lib The parent font library that will be used to generate the face
        * @param [in] path The path to the font resource
        */
        Font(Resources *res, FontLibrary &lib, const std::string &path);
        Font(Font &&other) = delete; // TEMP
        /**
        * Destruct Font.
        * If the `lib` parameter that was given to it upon
        * construction is deleted, then undefined behavior is invoked (IIRC).
        * @see Font(Resources *res, FontLibrary &lib, const std::string &path)
        */
        ~Font();
        // Font &operator=(Font &&other);
        /**
        * Set the pixel size of the font.
        * @param [in] size The pixel size
        */
        void setSize(unsigned int size);
        /**
        * Binds the given codepoint's texture to the currently
        * active OpenGL texture slot.
        * @param [in] c The codepoint to bind
        */
        void bindChar(char_t c);
        /**
        * Clears the codepoint texture cache.
        */
        void clearCache();

        /**
        * Gets the width of each codepoint
        * @return The width
        * @see charWidth
        */
        int getCharWidth();
        /**
        * Gets the height of each codepoint
        * @return The height
        * @see charHeight
        */
        int getCharHeight();
    };
}

#endif
