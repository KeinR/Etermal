#ifndef ETERMAL_SHADER_PRIMITIVE_H_INCLUDED
#define ETERMAL_SHADER_PRIMITIVE_H_INCLUDED

#include "Shader.h"

namespace etm { class Resources; }

namespace etm::shader {

    /**
    * Primitive shader interface.
    * Used for rendering 2D primitives (really only rectangles ATM)
    * @see ../../../resources/shaders/primitive.vert
    * @see ../../../resources/shaders/primitive.frag
    * @see Rectangle
    * @see Text
    */
    class Primitive: public Shader {
        /// Handle to a @ref Resources object.
        /// Used for error reporting.
        Resources *res;
        /// Location of the shader's model uniform
        uniform_t model;
        /// Location of the shader's color uniform
        uniform_t color;
    public:
        /**
        * Construct a primitive shader with
        * a @ref Resources object to report errors
        * to.
        * @param [in] res Resources object to report errors to
        */
        Primitive(Resources *res);

        uniform_t getModel() const override;
        uniform_t getColor() const override;
        /**
        * @warning Will fail, as Primitive has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getBackGColor() const override;
        /**
        * @warning Will fail, as Primitive has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getForeGColor() const override;
    };
}

#endif
