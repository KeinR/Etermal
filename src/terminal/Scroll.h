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

        void recalc();
    public:
        Scroll();

        void setNetHeight(int height);
        void setGrossHeight(int height);

        int getGrossHeight();
        float getOffset();
        float getMaxOffset();

        void scroll(float ammount);
        // Scroll to max offset
        void jump();
    };
}

#endif
