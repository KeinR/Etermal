#ifndef ETERMAL_CHARACTER_H_INCLUDED
#define ETERMAL_CHARACTER_H_INCLUDED

#include "render/Color.h"
#include "util/enums.h"

namespace etm {
    class Character {
        char value;
        Color color;
        charStyle::type style;
    public:
        // Zero initialize
        Character();
        // With args
        Character(char value);

        void setValue(char value);
        void setColor(const Color &color);

        char getValue() const;
        Color &getColor();
    };
}

#endif
