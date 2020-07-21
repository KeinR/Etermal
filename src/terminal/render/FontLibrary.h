#ifndef ETERMAL_FONTLIBRARY_H_INCLUDED
#define ETERMAL_FONTLIBRARY_H_INCLUDED

#include "../util/singleton.h"
#include "ftype.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {

    /**
    * Simple Freetype library wrapper that provides automatic
    * management of the library.
    */
    class FontLibrary: public singleton {
        /// Handle to the Freetype library
        FT_Library lib;
    public:
        /**
        * Construct a new Freetype font library.
        * @warning This library must be alive until all
        * its children (fonts that were made with it)
        * are deleted.
        * @param [in] res A @ref Resources object to witch constructor errors will be posted.
        */
        FontLibrary(Resources *res);
        /**
        * Destroy the freetype library
        * @see lib
        */
        ~FontLibrary();
        /**
        * Get the wrapped freetype library
        * @see lib
        */
        FT_Library get();
    };
}

#endif
