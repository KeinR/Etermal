#include "Primitive.h"

#include "../render/glfw.h"

#define MODEL "Model"
#define COLOR "Color"

etm::shader::Primitive::Primitive():
    Shader("resources/shaders/primitive.vert", "resources/shaders/primitive.frag"),
    model(glGetUniformLocation(get(), MODEL)),
    color(glGetUniformLocation(get(), COLOR)) {
}
etm::shader::uniform_t etm::shader::Primitive::getModel() const {
    return model;
}
etm::shader::uniform_t etm::shader::Primitive::getColor() const {
    return color;
}
