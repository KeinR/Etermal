#ifndef ETERMAL_RESOURCES_H_INCLUDED
#define ETERMAL_RESOURCES_H_INCLUDED

#include "render/Buffer.h"
#include "render/Font.h"
#include "render/FontLibrary.h"
#include "render/Texture.h"
#include "shader/Primitive.h"
#include "shader/Text.h"

namespace etm {
    // shader/Shader
    class Shader;
    // Terminal
    class Terminal;
    // util/termError
    class termError;
}

namespace etm {
    class Resources {
        Terminal *terminal;

        Buffer rectangle;
        Texture triangle;

        shader::Text textShader;
        shader::Primitive primitiveShader;
        shader::Shader *currentShader;

        FontLibrary fontLib;
        Font font;

        void genRectangle();
        void genTriangle();
    public:
        Resources(Terminal &terminal);

        void postError(const std::string &location, const std::string &message, int code, bool severe);

        void setTerminal(Terminal &terminal);
        Terminal &getTerminal();

        void renderRectangle();
        void renderTriangle();

        void bindTextShader();
        void bindPrimitiveShader();
        shader::Shader &getShader();

        Font &getFont();
    };
}

#endif
