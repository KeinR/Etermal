#ifndef ETERMAL_TM_LOOKBEHIND_H_INCLUDED
#define ETERMAL_TM_LOOKBEHIND_H_INCLUDED

#include "TextState.h"

// ../render/Color
namespace etm { class Color; }

namespace etm::tm {
    class Lookbehind: public TextState {
        const Color *const defBack;
        const Color *const defFore;
        unsigned int backLine;
        const Color *back;
        unsigned int foreLine;
        const Color *fore;
        unsigned int thisLine;
    public:
        Lookbehind(const Color &defBack, const Color &defFore, unsigned int lineNumber);
        void setDefBack() override;
        // Sets default foreground color, constant
        void setDefFore() override;

        // Set the background color
        void setBack(const Color &color) override;
        // Set the foreground color
        void setFore(const Color &color) override;

        // Returns true if the lookbehind can terminate;
        // Encountered a change in the background and foreground
        bool bothSet();
        const Color &getBack();
        const Color &getFore();
        // Decriment current line number by 1
        void decLine();

    };
}

#endif
