#ifndef ETERMAL_RECTANGLE_H_INCLUDED
#define ETERMAL_RECTANGLE_H_INCLUDED

#include "../render/Model.h"
#include "../render/Color.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {

    /**
    * A simple 2D primitive that wrapps positional,
    * scale, and color data.
    * Tied to a given @ref Resources object, as it renders
    * the buffer contained by it.
    */
    class Rectangle {
        /// Handle to the @ref Resources object.
        // Used in the @ref render() func.
        Resources *res;
        /// The model that describes the scale
        /// and position of the rectangle.
        Model model;
        /// Records the color of the rectangle.
        Color color;

    public:
        /**
        * Construct a rectangle
        * @note The given @ref Resources object must still
        * be alive when @ref render() is called.
        * @param [in] res A @ref Resources object
        */
        Rectangle(Resources *res);

        /**
        * Sets the color.
        * @param [in] color The color
        */
        void setColor(const Color &color);

        /**
        * Set the x coordinate of the model.
        * @param [in] x The x coordinate
        * @see getX()
        */
        void setX(float x);
        /**
        * Set the y coordinate of the model.
        * @param [in] y The y coordinate
        * @see getY()
        */
        void setY(float y);
        /**
        * Set the width of the model.
        * @param [in] width The width
        * @see getWidth()
        */
        void setWidth(float width);
        /**
        * Set the height of the model.
        * @param [in] height The height
        * @see getHeight()
        */
        void setHeight(float height);

        /**
        * Gets the x coordinate of the model.
        * @return The x coordiante
        * @see setX(float x)
        */
        float getX();
        /**
        * Gets the y coordinate of the model.
        * @return The y coordiante
        * @see setY(float y)
        */
        float getY();
        /**
        * Gets the width of the model.
        * @return The width
        * @see setWidth(float width)
        */
        float getWidth();
        /**
        * Gets the height of the model.
        * @return The height
        * @see setHeight(float height)
        */
        float getHeight();

        /**
        * Gets if the model has the point specified.
        * @param [in] x The x value
        * @param [in] y The y value
        * @return `true` if the model contains the point
        */
        bool hasPoint(float x, float y);

        /**
        * Renders the Rectangle to the current framebuffer
        * using the positional information in @ref model,
        * color info in @ref color, and render data in @ref res.
        * @note Requires that the primitive shader in @ref res
        * be set [@ref Resources::bindPrimitiveShader].
        * @note After this call, the color and model uniforms will
        * be modified.
        * @note Will fail if @ref Resources::genRectangle() failed
        */
        void render();

    };
}

#endif
