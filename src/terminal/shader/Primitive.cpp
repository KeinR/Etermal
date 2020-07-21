#include "Primitive.h"

#include "../render/opengl.h"
#include "../Resources.h"

// Names of uniforms (for lookup)
static const char *MODEL = "Model";
static const char *COLOR = "Color";

// Shaders compiled into a single header to enable embedding
#include "../../../resources/shaders/primitive.h"

etm::shader::Primitive::Primitive(Resources *res):
    Shader(res, primitive_vert, primitive_vert_len, primitive_frag, primitive_frag_len),
    res(res),
    model(glGetUniformLocation(get(), MODEL)),
    color(glGetUniformLocation(get(), COLOR)) {
}
etm::shader::uniform_t etm::shader::Primitive::getModel() const {
    return model;
}
etm::shader::uniform_t etm::shader::Primitive::getColor() const {
    return color;
}
etm::shader::uniform_t etm::shader::Primitive::getBackGColor() const {
    res->postError(
        "etm::shader::Text::getBackGColor: ",
        "Primitive shader does not have a background color uniform!\n"
        "Primitive shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}
etm::shader::uniform_t etm::shader::Primitive::getForeGColor() const {
    res->postError(
        "etm::shader::Text::getForeGColor: ",
        "Primitive shader does not have a foreground color uniform!\n"
        "Primitive shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}
