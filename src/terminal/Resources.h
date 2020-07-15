#ifndef ETERMAL_RESOURCES_H_INCLUDED
#define ETERMAL_RESOURCES_H_INCLUDED

#include "render/Buffer.h"
#include "render/Font.h"
#include "render/FontLibrary.h"
#include "shader/Primitive.h"
#include "shader/Text.h"

namespace etm {
    class Shader;
    class Terminal;
}

namespace etm {
    class Resources {
        Terminal *terminal;

        Buffer rectangle;

        shader::Text textShader;
        shader::Primitive primitiveShader;
        shader::Shader *currentShader;

        FontLibrary fontLib;
        Font font;

        void genRectangle();
    public:
        Resources(Terminal &terminal);

        void setTerminal(Terminal &terminal);
        Terminal &getTerminal();

        void renderRectangle();

        void bindTextShader();
        void bindPrimitiveShader();
        shader::Shader &getShader();

        Font &getFont();
    };
}

#endif
