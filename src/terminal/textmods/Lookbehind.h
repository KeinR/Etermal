#ifndef ETERMAL_TM_LOOKBEHIND_H_INCLUDED
#define ETERMAL_TM_LOOKBEHIND_H_INCLUDED

#include "TextState.h"

// ../render/Color
namespace etm { class Color; }

namespace etm::tm {

    /**
    * TextState Implementation that is used during the
    * lookbehind phase during rendering to determine the
    * currently active color values.
    * @see RenderState
    * @see TextBuffer
    */
    class Lookbehind: public TextState {
        /// The default background color
        const Color *const defBack;
        /// The default foreground color
        const Color *const defFore;
        /// Line on which the @ref back - ground color was modified
        unsigned int backLine;
        /// The set background color
        const Color *back;
        /// Line on which the @ref fore - ground color was modified
        unsigned int foreLine;
        /// The set foreground color
        const Color *fore;
        /// The current line
        unsigned int thisLine;
    public:
        /**
        * Construct a Lookbehind.
        * @param [in] defBack The default background color
        * @param [in] defFore The default foreground color
        * @param [in] lineNumber The current line number
        */
        Lookbehind(const Color &defBack, const Color &defFore, unsigned int lineNumber);
        /**
        * Calls @ref setBack(const Color &color) with @ref defBack
        */
        void setDefBack() override;
        /**
        * Calls @ref setFore(const Color &color) with @ref defFore
        */
        void setDefFore() override;

        /**
        * Sets the background color, only if it's not been set
        * or @ref thisLine == @ref backLine.
        * @param [in] color The desired color
        */
        void setBack(const Color &color) override;
        /**
        * Sets the foreground color, only if it's not been set
        * or @ref thisLine == @ref foreLine.
        * @param [in] color The desired color
        */
        void setFore(const Color &color) override;

        /**
        * Check if the lookbehind can terminate;
        * that is, if both the foreground and background colors
        * have been set.
        * @return `true` if the lookbehind can terminate
        * @see setBack(const Color &color)
        * @see setFore(const Color &color)
        */
        bool bothSet();
        /**
        * Retrieves the background color.
        * If the background color hasn't been set, returns @ref defBack
        * @return The background color
        * @see getFore()
        */
        const Color &getBack();
        /**
        * Retrieves the foreground color.
        * If the foreground color hasn't been set, returns @ref defBack
        * @return The foreground color
        * @see getBack()
        */
        const Color &getFore();
        /**
        * Decriment the line number [@ref thisLine] by 1
        */
        void decLine();

    };
}

#endif
