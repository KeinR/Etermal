#ifndef ETERMAL_FONTLIBRARY_H_INCLUDED
#define ETERMAL_FONTLIBRARY_H_INCLUDED

#include "../util/singleton.h"

#include "ftype.h"
// class FT_Library;

namespace etm {
    class FontLibrary: public singleton {
        FT_Library lib;
    public:
        FontLibrary();
        ~FontLibrary();
        FT_Library get();
    };
}

#endif
