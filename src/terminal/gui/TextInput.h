#ifndef ETERMAL_TEXTINPUT_H_INCLUDED
#define ETERMAL_TEXTINPUT_H_INCLUDED

#include "Text.h"
#include "../util/enums.h"
#include "Rectangle.h"

namespace etm { class Terminal; }


// Highlighting text - copy/paste
// pasting
// insertion

namespace etm {
    class TextInput {
        Resources *res;
        Text text;
        Rectangle container;
        std::string::size_type cursorIndex;
        std::string str;
        // TextInput won't respond to input events unless focused
        // (with the exception of mouseClick, because that's what determines
        // the focused state)
        bool focused;

        void moveCursor(int dir);
        void truncCursor();
    public:
        TextInput(Resources *res);

        void setFocused(bool value);

        // Input events
        void action(actionKey key);
        void insertChar(char c);
        void insertString(const std::string &str);
        void mouseClick(float mouseX, float mouseY);

        void setX(float x);
        void setY(float y);
        void setWidth(float width);
        void setHeight(float height);

        void setColor(const Color &color);
        void setString(const std::string &str);

        void update();

        void render();
    };
}

#endif
