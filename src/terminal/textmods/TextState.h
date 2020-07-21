#ifndef ETERMAL_TEXTSTATE_H_INCLUDED
#define ETERMAL_TEXTSTATE_H_INCLUDED

// ../render/Color
namespace etm { class Color; }

/*
* The state of the text as it's being rendered.
* Modified by multible extensions of etm::tm::Mod
*/

namespace etm::tm {

    /**
    * The state of the text as it's being processed.
    * Modified by multible extensions of @ref Mod.
    */
    class TextState {
    public:
        virtual ~TextState() = 0;

        /**
        * Sets the background color to the default.
        */
        virtual void setDefBack() = 0;
        /**
        * Sets the foreground color to the default.
        */
        virtual void setDefFore() = 0;

        /**
        * Sets the background color.
        * @param [in] color The new background color
        */
        virtual void setBack(const Color &color) = 0;
        /**
        * Sets the foreground color.
        * @param [in] color The new foreground color
        */
        virtual void setFore(const Color &color) = 0;
    };
}

#endif
