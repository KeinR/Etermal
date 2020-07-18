#ifndef ETERMAL_BUTTON_H_INCLUDED
#define ETERMAL_BUTTON_H_INCLUDED

#include <functional>

#include "Rectangle.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {
    class Button {
    public:
        typedef std::function<void()> callback_t;
    private:
        Resources *res;

        Model area;
        bool hovering;

        callback_t onMousePress;
        callback_t onMouseRelease;
        callback_t onMouseEnter;
        callback_t onMouseLeave;

    public:
        Button(Resources *res);

        void setX(float x);
        void setY(float y);
        void setWidth(float width);
        void setHeight(float height);

        float getX();
        float getY();
        float getWidth();
        float getHeight();

        // Set the functions that will be invoked should
        // certain events occur
        void setOnMousePress(const callback_t &callback);
        void setOnMouseRelease(const callback_t &callback);
        void setOnMouseEnter(const callback_t &callback);
        void setOnMouseLeave(const callback_t &callback);

        // Input callbacks, called to relay events.
        // Not garunteed to trigger the callbacks.
        void mouseClick(bool isPressed, float mouseX, float mouseY);
        void mouseMove(float mouseX, float mouseY);

        // Needs primitive shader to be set
        void render();
    };
}

#endif
