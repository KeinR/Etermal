#ifndef ETERMAL_CHARACTER_H_INCLUDED
#define ETERMAL_CHARACTER_H_INCLUDED

#include "render/Color.h"
#include "util/enums.h"

namespace etm {
    class Glyph {
    public:
        int index;
        Color color;
        charStyle::type style;
        // Zero initialize
        Glyph();
        // With args
        Glyph(int index, const Color &color, charStyle::type style);
    };
}

#endif
