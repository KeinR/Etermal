#ifndef ETERMAL_FONT_H_INCLUDED
#define ETERMAL_FONT_H_INCLUDED

#include <string>
#include <map>

#include "EtmFont.h"
#include "ftype.h"
#include "Texture.h"
#include "FontLibrary.h"

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
    class Font: public EtmFont {
        /// The text cache type
        typedef std::map<char_t, Texture> textCache_t;
        FontLibrary fontLib;

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
        /**
        * Move a font into this font.
        * @param [in,out] other Font to move
        */
        void steal(Font &other);
    public:
        /**
        * Construct a Font.
        * Contains a root font library object, which is a good and bad thing.
        * @param [in] path The path to the font resource
        * @throw std::logic_error if failed to create freetype library
        * @throw std::invalid_argument if failed to create font face from given `path`
        */
        Font(const std::string &path);
        /**
        * Initialize with moved object.
        * @param [in,out] other Moved object
        */
        Font(Font &&other);
        /**
        * Destruct Font.
        * If the `lib` parameter that was given to it upon
        * construction is deleted, then undefined behavior is invoked (IIRC).
        * @see Font(Resources *res, FontLibrary &lib, const std::string &path)
        */
        ~Font();
        /**
        * Assign to moved object.
        * @param [in,out] other Moved object
        */
        Font &operator=(Font &&other);

        void setResMan(Resources *res) override;
        void setSize(unsigned int size) override;
        void bindChar(char_t c) override;
        void clearCache() override;
        int getCharWidth() override;
        int getCharHeight() override;
    };
}

#endif
