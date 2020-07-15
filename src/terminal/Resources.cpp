#include "Resources.h"

#include "render/ftype.h"
#include "util/error.h"

static void initRectangle(etm::Buffer &buffer);

void initRectangle(etm::Buffer &buffer) {
    buffer.setParam(0, 2, 4, 0);
    buffer.setParam(1, 2, 4, 2);
}

etm::Resources::Resources(Terminal &terminal):
    terminal(&terminal),
    rectangle(initRectangle),
    font(fontLib, "C:\\Windows\\Fonts\\lucon.ttf")
{

    genRectangle();
    bindPrimitiveShader(); // Default setting to avoid unfortunate events
}

void etm::Resources::genRectangle() {
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

    rectangle.setVerticies(16, vertices);
    rectangle.setIndices(6, indices);
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

void etm::Resources::bindTextShader() {
    currentShader = &textShader;
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
