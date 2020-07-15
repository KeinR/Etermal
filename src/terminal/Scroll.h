#ifndef ETERMAL_SCROLL_H_INCLUDED
#define ETERMAL_SCROLL_H_INCLUDED

/*
* Data class used to keep track of scrolling
*/

namespace etm {
    class Scroll {
        // Total height of all elements
        int grossHeight;
        // Actual displayed height
        int netHeight;
        float offset;
        // Maximum scroll offset
        float maxOffset;

        // Scroll units
        float align;
        // Scroll unit progress
        float alignBuffer;

        void recalc();
    public:
        Scroll();

        // Must be positive and not equal to zero
        void setAlign(float value);

        void setNetHeight(int height);
        void setGrossHeight(int height);

        int getNetHeight();
        int getGrossHeight();
        float getOffset();
        float getMaxOffset();

        // Returns true if actually scrolled
        bool scroll(float ammount);
        // Scroll to max offset
        void jump();
    };
}

#endif
