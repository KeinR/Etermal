#ifndef ETERMAL_ETMFONT_H_INCLUDED
#define ETERMAL_ETMFONT_H_INCLUDED

namespace etm {
    // ../Resource
    class Resources;
}

namespace etm {
    class EtmFont {
    public:
        /// Codepoint type
        typedef unsigned int char_t;

        virtual ~EtmFont() = 0;

        /** @internal
        * Sets the resources manager, for error reporting
        * @param [in] The manager
        */
        virtual void setResMan(Resources *res) = 0;

        /**
        * Set the pixel size of the font.
        * @param [in] size The pixel size
        */
        virtual void setSize(unsigned int size) = 0;
        /**
        * Binds the given codepoint's texture to the currently
        * active OpenGL texture slot.
        * @param [in] c The codepoint to bind
        */
        virtual void bindChar(char_t c) = 0;
        /**
        * Clears the codepoint texture cache.
        */
        virtual void clearCache() = 0;

        /**
        * Gets the width of each codepoint
        * @return The width
        * @see charWidth
        */
        virtual int getCharWidth() = 0;
        /**
        * Gets the height of each codepoint
        * @return The height
        * @see charHeight
        */
        virtual int getCharHeight() = 0;
    };
}

#endif
