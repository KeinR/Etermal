#ifndef ETERMAL_ACTIONKEY_H_INCLUDED
#define ETERMAL_ACTIONKEY_H_INCLUDED

namespace etm {
    enum actionKey {
        ENTER,
        BACKSPACE,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
    struct charStyle {
        typedef int type;
        static constexpr type UNDERLINE = 1 << 0;
        static constexpr type BOLD = 1 << 1;
        static constexpr type ITALIC = 1 << 2;
    };
}

#endif
