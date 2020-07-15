#include "Primitive.h"

#include "../render/glfw.h"
#include "../util/error.h"

#define MODEL "Model"
#define COLOR "Color"

#include "../../../resources/shaders/primitive.h"

etm::shader::Primitive::Primitive():
    Shader(primitive_vert, primitive_vert_len, primitive_frag, primitive_frag_len),
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
    throw fe_error(
    "etm::shader::Text::getBackGColor: "
    "Primitive shader does not have a background color uniform!\n"
    "Primitive shader has been erroniously set.\n"
    "This is a bug.");
}
etm::shader::uniform_t etm::shader::Primitive::getForeGColor() const {
    throw fe_error(
    "etm::shader::Text::getForeGColor: "
    "Primitive shader does not have a foreground color uniform!\n"
    "Primitive shader has been erroniously set.\n"
    "This is a bug.");
}
