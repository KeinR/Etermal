#ifndef ETERMAL_TRIANGLE_H_INCLUDED
#define ETERMAL_TRIANGLE_H_INCLUDED

#include "../render/RModel.h"
#include "../render/Color.h"

// ../Resources
namespace etm { class Resources; }

namespace etm {

    /**
    * Represents a simple 2D triangle primitive.
    * This one is very different from @ref Rectangle
    * however in that it renders as an anti-aliased
    * texture made in @ref Resources.
    * Also unlike Rectangle, can be rotated.
    */
    class Triangle {
        /// Handle to the @ref Resources object
        Resources *res;
        /// The model that describes the position,
        /// size, and rotation of the triangle.
        RModel model;
        /// The background color of the texture
        /// (the part that isn't triangle)
        /// @see foregroundColor
        Color backgroundColor;
        /// The foreground color of the texture.
        /// @see backgroundColor
        Color foregroundColor;
    public:
        /**
        * Construct a Triangle.
        * @param [in] res A @ref Resources object
        */
        Triangle(Resources *res);

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
        * Set the rotation of the texture.
        * @param [in] degrees The rotation in degrees
        * @see getRotation()
        */
        void setRotation(float degrees);

        /**
        * Set the background color
        * @param [in] color The color
        * @see backgroundColor
        * @see setForegroundColor(const Color &color)
        */
        void setBackColor(const Color &color);

        /**
        * Set the foreground color
        * @param [in] color The color
        * @see foregroundColor
        * @see setBackColor(const Color &color)
        */
        void setForeColor(const Color &color);

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
        * Set the rotation of the texture.
        * @return The rotation in degrees
        * @see getRotation()
        */
        float getRotation();

        /**
        * Render the triangle to the current framebuffer using the
        * back/foreground colors, model, and @ref Resources handle.
        * @note The text shader must be set in @ref res
        * [@ref Resources::bindTextShader()]
        * @note The model and fore/background color uniforms in the
        * shader will be modified.
        * @note Will fail if @ref Resources::genTriangle() failed
        */
        void render();
    };
}

#endif
