#ifndef ETERMAL_RESOURCES_H_INCLUDED
#define ETERMAL_RESOURCES_H_INCLUDED

#include "render/Buffer.h"
#include "render/Font.h"
#include "shader/Primitive.h"
#include "shader/Texture.h"

namespace etm {
    class Shader;
    class Terminal;
}

namespace etm {
    class Resources {
        Terminal *terminal;

        Buffer rectangle;
        Buffer texRectangle;

        shader::Texture textureShader;
        shader::Primitive primitiveShader;
        shader::Shader *currentShader;

        Font::FT_Library_for fontLib;
        Font font;

        void genRectangle();
        void genTexRectangle();
    public:
        Resources(Terminal &terminal);
        ~Resources();

        void setTerminal(Terminal &terminal);
        Terminal &getTerminal();

        void renderRectangle();
        void renderTexRectangle();

        void bindTextureShader();
        void bindPrimitiveShader();
        shader::Shader &getShader();

        Font &getFont();
    };
}

#endif
