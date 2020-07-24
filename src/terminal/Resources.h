#ifndef ETERMAL_RESOURCES_H_INCLUDED
#define ETERMAL_RESOURCES_H_INCLUDED

#include "render/Buffer.h"
#include "render/Font.h"
#include "render/FontLibrary.h"
#include "render/Texture.h"
#include "shader/Primitive.h"
#include "shader/Text.h"
#include "shader/Texture.h"

namespace etm {
    // shader/Shader
    class Shader;
    // Terminal
    class Terminal;
    // util/termError
    class termError;
}

namespace etm {

    /**
    * Takes care of all resource management for the @ref Terminal
    */
    class Resources {
        /// Handle to parent @ref Terminal
        Terminal *terminal;

        /// Basic 2D quad
        Buffer rectangle;
        /// Anti-aliased triangle texture
        Texture triangle;

        /// @see shader::Text
        shader::Text textShader;
        /// @see shader::Primitive
        shader::Primitive primitiveShader;
        /// @see shader::Texture
        shader::Texture textureShader;
        /// The active shader.
        /// Only ever bound if the shader is made
        /// current (bound via OpenGL calls) as well.
        shader::Shader *currentShader;

        /// Font root object
        FontLibrary fontLib;
        /// Used font
        Font font;

        /// Generate @ref rectangle
        void genRectangle();
        /// Generate @ref triangle
        void genTriangle();
    public:
        /**
        * Construct a new object with parent.
        * @param [in] terminal The parent terminal
        */
        Resources(Terminal &terminal);

        /**
        * Nofiy `*this` of an error.
        * The data given is packaged into a @ref termError object
        * and shipped off to @ref terminal.
        */
        void postError(const std::string &location, const std::string &message, int code, bool severe);

        /**
        * Sets the parent @ref terminal.
        * @param [in] terminal The new terminal
        * @see getTerminal()
        */
        void setTerminal(Terminal &terminal);
        /**
        * Gets the parent @ref terminal.
        * @return The parent terminal
        * @see setTerminal(Terminal &terminal)
        */
        Terminal &getTerminal();

        /**
        * Renders the rectangle buffer to the current
        * framebuffer.
        * @note This doesn't set any shaders, only calls
        * render on @ref rectangle. Because of this, needs
        * the primitive shader to be set [@ref bindPrimitiveShader()]
        */
        void renderRectangle();
        /**
        * Renders the triangle texture to the current
        * framebuffer.
        * @note Because it's just a texture, @ref triangle
        * is first bound, then @ref renderRectangle() is called.
        * @note Doesn't set any shaders. Requires that the text
        * shader be set [@ref bindTextShader()]
        */
        void renderTriangle();

        /**
        * Binds the @ref textShader, making it
        * the current OpenGL shader program, and setting
        * it as the return value of @ref getShader().
        */
        void bindTextShader();
        /**
        * Binds the @ref primitiveShader, making it
        * the current OpenGL shader program, and setting
        * it as the return value of @ref getShader().
        */
        void bindPrimitiveShader();
        /**
        * Binds the @ref textureShader, making it
        * the current OpenGL shader program, and setting
        * it as the return value of @ref getShader().
        */
        void bindTextureShader();
        /**
        * Gets the currently bound shader.
        * @note Will never be `nullptr`. Will always either be
        * @ref textShader or @ref primitiveShader
        * @return The shader
        */
        shader::Shader &getShader();

        /**
        * Get the in-use @ref font.
        * @return The font
        * @see Font
        */
        Font &getFont();
    };
}

#endif
