#ifndef ETERMAL_SCROLLBAR_H_INCLUDED
#define ETERMAL_SCROLLBAR_H_INCLUDED

#include "Rectangle.h"
#include "Triangle.h"
#include "Button.h"
#include "../util/Timer.h"

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

        class Arrow {
            Scrollbar *parent;
            Color bgColor;
            Color arrowColor;
            Triangle arrow;
            Button button;
            int directionMod;
            float arrowSize;
            bool pressed;
            bool active;
            Timer tick;
            Timer wait;
            void calcArrowW();
            void calcArrowH();
        public:
            // Arrow size as a proportion of the container (0-1)
            Arrow(Scrollbar *parent, float arrowSize, int tickTimeMillis, int arrowWaitMillis, int directionMod);

            void setX(float x);
            void setY(float y);
            void setWidth(float width);
            void setHeight(float height);

            float getX();
            float getY();
            float getWidth();
            float getHeight();

            void setColor(const Color &color);
            void setArrowColor(const Color &color);
            void mouseClick(bool isPressed, float mouseX, float mouseY);
            void mouseMove(float mouseX, float mouseY);

            void setActive(bool value);

            void render();
        };

        Arrow upArrow;
        Arrow downArrow;

        void calcSliderX();
        void calcSliderY();
        void calcSliderWidth();
        void calcArrows();
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
