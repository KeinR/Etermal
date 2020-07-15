#ifndef ETERMAL_CHARACTER_H_INCLUDED
#define ETERMAL_CHARACTER_H_INCLUDED

#include "render/Color.h"
#include "util/enums.h"

namespace etm {
    class Character {
        char value;
        Color backGColor;
        bool enforceBackColor;
        Color foreGColor;
        bool enforceForeColor;
        charStyle::type style;
    public:
        // Zero initialize
        Character();
        // With args
        Character(char value);

        void setValue(char value);
        void setBackColor(const Color &color);
        void setForeColor(const Color &color);

        char getValue() const;

        bool hasBackGC();
        bool hasForeGC();
        Color &getBackGC();
        Color &getForeGC();
    };
}

#endif
