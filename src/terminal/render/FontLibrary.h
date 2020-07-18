#ifndef ETERMAL_FONTLIBRARY_H_INCLUDED
#define ETERMAL_FONTLIBRARY_H_INCLUDED

#include "../util/singleton.h"
#include "ftype.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {
    class FontLibrary: public singleton {
        FT_Library lib;
    public:
        FontLibrary(Resources *res);
        ~FontLibrary();
        FT_Library get();
    };
}

#endif
