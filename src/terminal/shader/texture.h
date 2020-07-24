#ifndef ETERMAL_SHADER_TEXTURE_H_INCLUDED
#define ETERMAL_SHADER_TEXTURE_H_INCLUDED

#include "Shader.h"

namespace etm { class Resources; }

namespace etm::shader {

    /**
    * Texture shader interface.
    * Used for rendering single milti-channel textures (GL_RGB).
    * @see ../../../resources/shaders/texture.vert
    * @see ../../../resources/shaders/texture.frag
    * @see Text
    * @see Primitive
    */
    class Texture: public Shader {
        /// Handle to a @ref Resources object.
        /// Used for error reporting.
        Resources *res;
        /// Location of the shader's model uniform
        uniform_t model;
    public:
        /**
        * Construct a texture shader with a @ref Resources
        * object to report errors to.
        * @param [in] res Resources object to report errors to
        */
        Texture(Resources *res);

        uniform_t getModel() const override;
        /**
        * @warning Will fail, as Texture has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getColor() const override;
        /**
        * @warning Will fail, as Texture has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getForeGColor() const override;
        /**
        * @warning Will fail, as Texture has no such uniform.
        * An error will be reported to @ref res.
        * @return -1
        */
        uniform_t getBackGColor() const override;
    };
}

#endif
