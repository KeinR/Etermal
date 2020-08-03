#ifndef ETERMAL_BMPFONT_H_INCLUDED
#define ETERMAL_BMPFONT_H_INCLUDED

#include <vector>
#include <string>

#include "EtmFont.h"
#include "Texture.h"

namespace etm {

    /**
    * Class representing a bitmap font.
    * @see Font
    */
    class BmpFont: public EtmFont {
        char_t startCodepoint;
        std::vector<Texture> glyphs;
        int width;
        int height;
    public:
        /**
        * Create a bitmap font.
        * @warning When created, will fill texture objects, binding
        * itself to the current context. Make sure the desired context
        * is current!
        * @note The first glyph in the bitmap
        * is interpreted as the null character - 
        * the char that will be displayed if the
        * given codepoint is out of range.
        * @note Expects the given image to have only two channels
        * (be black and white)
        * @param [in] path Path to the bitmap
        * @param [in] startCodepoint The value of the starting codepoint.
        * This will be the second glyph in the list, for previously stated
        * reasons.
        * @param [in] glyphWidth The pixel width of each glyph
        * @param [in] glyphHeight The pixel height of each glyph
        * @param [in] count The number of glyphs contained within the
        * bitmap, not including the first "null" glyph.
        * @throw std::invalid_argument if the given `path` cannot be opened or decoded
        */
        BmpFont(
            const std::string &path,
            char_t startCodepoint,
            char_t glyphWidth,
            char_t glyphHeight,
            char_t count
        );

        /**
        * Does nothing, we don't need it.
        * @param [in] res [ignored]
        */
        void setResMan(Resources *res) override;

        /**
        * Does nothing, partly because bitmap fonts aren't
        * meant to be scalable (and look like shit when they are),
        * but also because the class's memory footprint would
        * have to double in order to support scaling.
        * @param [in] size [ignored]
        */
        void setSize(unsigned int size) override;
        void bindChar(char_t c) override;

        /**
        * Does nothing, as there's no cache.
        */
        void clearCache() override;
        int getCharWidth() override;
        int getCharHeight() override;
    };
}

#endif
