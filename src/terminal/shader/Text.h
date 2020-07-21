#ifndef ETERMAL_SHADER_TEXT_H_INCLUDED
#define ETERMAL_SHADER_TEXT_H_INCLUDED

#include "Shader.h"

namespace etm { class Resources; }

namespace etm::shader {

    /**
    * Text shader interface.
    * Used for rendering single channel textures (GL_RED),
    * namely @e text.
    * @see ../../../resources/shaders/text.vert
    * @see ../../../resources/shaders/text.frag
    * @see Texture
    * @see Primitive
    */
    class Text: public Shader {
        /// Handle to a @ref Resources object.
        /// Used for error reporting.
        Resources *res;
        /// Location of the shader's background color uniform
        uniform_t backgroundColor;
        /// Location of the shader's foreground color uniform
        uniform_t foregroundColor;
        /// Location of the shader's model uniform
        uniform_t model;
    public:
        /**
        * Construct a text shader with a @ref Resources
        * object to report errors to.
        * @param [in] res Resources object to report errors to
        */
        Text(Resources *res);

        uniform_t getModel() const override;
        /**
        * @warning Will fail, as Text has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getColor() const override;
        uniform_t getForeGColor() const override;
        uniform_t getBackGColor() const override;
    };
}

#endif
