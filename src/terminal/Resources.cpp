#include "Resources.h"

#include "render/ftype.h"
#include "util/error.h"

static void initRectangle(etm::Buffer &buffer);
static void initTexRectangle(etm::Buffer &buffer);

void initRectangle(etm::Buffer &buffer) {
    buffer.setParam(0, 2, 2, 0);
}

void initTexRectangle(etm::Buffer &buffer) {
    buffer.setParam(0, 2, 4, 0);
    buffer.setParam(1, 2, 4, 2);
}

etm::Resources::Resources(Terminal &terminal):
    terminal(&terminal),
    rectangle(initRectangle),
    texRectangle(initTexRectangle),
    font(fontLib.get(), "C:\\Windows\\Fonts\\calibri.ttf")
{

    genRectangle();
    genTexRectangle();
    bindPrimitiveShader(); // Default setting to avoid unfortunate events
}

void etm::Resources::genRectangle() {
    float verticies[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f
    };

    unsigned int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    rectangle.setVerticies(8, verticies);
    rectangle.setIndices(6, indices);
}
void etm::Resources::genTexRectangle() {
    float vertices[16] = {
        // positions   // texture coords
        1.0,   1.0,  1.0f, 1.0f, // top right
        1.0,  -1.0,  1.0f, 0.0f, // bottom right
        -1.0, -1.0,  0.0f, 0.0f, // bottom left
        -1.0,  1.0,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    texRectangle.setVerticies(16, vertices);
    texRectangle.setIndices(6, indices);
}

void etm::Resources::setTerminal(Terminal &terminal) {
    this->terminal = &terminal;
}
etm::Terminal &etm::Resources::getTerminal() {
    return *terminal;
}

void etm::Resources::renderRectangle() {
    rectangle.render();
}
void etm::Resources::renderTexRectangle() {
    texRectangle.render();
}

void etm::Resources::bindTextureShader() {
    currentShader = &textureShader;
    currentShader->use();
}
void etm::Resources::bindPrimitiveShader() {
    currentShader = &primitiveShader;
    currentShader->use();
}
etm::shader::Shader &etm::Resources::getShader() {
    return *currentShader;
}

etm::Font &etm::Resources::getFont() {
    return font;
}
