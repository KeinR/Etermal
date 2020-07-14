#ifndef ETERMAL_SCROLLBAR_H_INCLUDED
#define ETERMAL_SCROLLBAR_H_INCLUDED

#include "Rectangle.h"

namespace etm {
    // ../Scroll
    class Scroll;
    // ../Resources
    class Resources;
}

namespace etm {
    class Scrollbar {
        Resources *res;
        Scroll *scroll;
        Rectangle bar;
        Color sliderColor;
        Color sliderColorHover;
        Rectangle slider;
        // Is mouse dragging?
        bool dragging;
        // The last drag point (we only track vertical)
        float dragY;
        // If there's no overflow, should be false - 
        // whether the scrollbar will render or not
        // upon calls to render()
        bool showingSlider;
        // If hovering, slider should highlight
        bool sliderHovering;

        int sideMargin;

        void calcSliderX();
        void calcSliderY();
        void calcSliderWidth();
    public:
        Scrollbar(Resources *res, Scroll &scroll);

        void setX(int x);
        void setY(int y);
        void setWidth(int width);
        void setHeight(int height);
        void setSideMargin(int margin);

        int getX();
        int getY();
        int getWidth();
        int getHeight();
        int getSideMargin();

        void setBarColor(const Color &color);
        void setSliderColor(const Color &color);

        // Must be called for slider to update.
        // Call when there's a change in the net height or scroll.
        // If Scrollbar induced the scroll, don't need to call update.
        void update();

        void mouseClick(bool isPressed, float mouseX, float mouseY);
        void mouseMove(float mouseX, float mouseY);

        // Needs primitive shader to be set!!
        void render();
    };
}

#endif
