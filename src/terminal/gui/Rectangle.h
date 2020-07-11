#ifndef ETERMAL_RECTANGLE_H_INCLUDED
#define ETERMAL_RECTANGLE_H_INCLUDED

#include "../render/Model.h"
#include "../render/Color.h"

namespace etm { class Resources; }

namespace etm {
    class Rectangle {
        Resources *res;
        Model model;
        Color color;

    public:
        Rectangle(Resources *res);

        void setColor(const Color &color);

        void setX(float x);
        void setY(float y);
        void setWidth(float width);
        void setHeight(float height);

        float getX();
        float getY();
        float getWidth();
        float getHeight();

        bool hasPoint(float x, float y);

        void render();

    };
}

#endif
