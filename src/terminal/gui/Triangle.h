#ifndef ETERMAL_TRIANGLE_H_INCLUDED
#define ETERMAL_TRIANGLE_H_INCLUDED

#include "../render/RModel.h"
#include "../render/Color.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {
    class Triangle {
        Resources *res;
        RModel model;
        Color backgroundColor;
        Color foregroundColor;
    public:
        Triangle(Resources *res);

        void setX(float x);
        void setY(float y);
        void setWidth(float width);
        void setHeight(float height);
        void setRotation(float degrees);
        void setBackColor(const Color &color);
        void setForeColor(const Color &color);

        float getX();
        float getY();
        float getWidth();
        float getHeight();
        float getRotation();

        void render();
    };
}

#endif
