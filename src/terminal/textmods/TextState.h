#ifndef ETERMAL_TEXTSTATE_H_INCLUDED
#define ETERMAL_TEXTSTATE_H_INCLUDED

// ../render/Color
namespace etm { class Color; }

/*
* The state of the text as it's being rendered.
* Modified by multible extensions of etm::tm::Mod
*/

namespace etm::tm {
    class TextState {
    public:
        virtual ~TextState() = 0;

        // Sets default background color, constant
        virtual void setDefBack() = 0;
        // Sets default foreground color, constant
        virtual void setDefFore() = 0;

        // Set the background color
        virtual void setBack(const Color &color) = 0;
        // Set the foreground color
        virtual void setFore(const Color &color) = 0;
    };
}

#endif
