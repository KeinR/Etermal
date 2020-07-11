#ifndef SCROLLBOX_H_INCLUDED
#define SCROLLBOX_H_INCLUDED

#include <vector>

#include "../util/singleton.h"
#include "../render/Model.h"
#include "Rectangle.h"

namespace etm {
    class Resources;
    class Text;
}

/*
* Vertical scroll
*/

namespace etm {
    class ScrollBox: public singleton {
    public:
        typedef std::vector<Text*> elements_t;
    private:
        Resources *res;

        Model model;
        elements_t elements;
        // Scroll position, from the top
        double scrollOffset;
        // Pixels between the elements
        int elementSpacing;
        // Pixels between the elements and the side
        int sideSpacing;
        // Mouse hovering, activated
        bool hovering;
        // Last recorded collective height of all elements
        int lastCollectiveHeight;
        // Maximum scroll offset
        double maxScrollOffest;

        class Scrollbar {
            ScrollBox *wrapper;
            Rectangle bar;
            // Is mouse dragging?
            bool dragging;
            // The last drag point (we only track vertical)
            double dragY;
            // If there's no overflow, should be false - 
            // whether the scrollbar will render or not
            // upon calls to render()
            bool showing;
        public:
            // Wrapper is the parent
            Scrollbar(ScrollBox &wrapper);
            void setWidth(int width);
            void setColor(const Color &color);
            void init(int scrollOffset, int collectiveHeight);
            void mouseClick(bool isPressed, float mouseX, float mouseY);
            void mouseMoved(float mouseX, float mouseY);
            void render();
        };

        Scrollbar scrollbar;

        // Scrolls the contents by `value` vertically
        void scroll(float value);
    public:
        ScrollBox(Resources *res);

        // Position and size info
        void setX(int x);
        void setY(int y);
        void setHeight(int height);
        void setWidth(int width);
        int getX();
        int getY();
        int getHeight();
        int getWidth();

        // Sets the width of the scrollbar
        void setScrollbarWidth(int width);
        // Sets the color of the scrollbar
        void setScrollbarColor(const Color &color);
        // The rendered elements
        elements_t &getElements();
        // Re-position the elements according to
        // the list
        void update();
        // Update only the y positions of the elements
        // (more efficient)
        void updateScroll();
        // Sets its own shaders
        void render();

        // Mouse position relative to viewport
        void mouseMoved(float mouseX, float mouseY);
        // Right mouse button
        void mouseClick(bool isPressed, float mouseX, float mouseY);
        void mouseScroll(float yOffset);
    };
}

#endif
