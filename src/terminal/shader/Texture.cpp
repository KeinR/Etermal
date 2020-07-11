#include "Texture.h"

#include "../util/error.h"
#include "../render/glfw.h"

#define MODEL "Model"
#define SAMPLER0 "Tex"

etm::shader::Texture::Texture():
    Shader("resources/shaders/texture.vert", "resources/shaders/texture.frag"),
    model(glGetUniformLocation(get(), MODEL))
{
    use();
    glUniform1i(glGetUniformLocation(get(), SAMPLER0), 0);
}
etm::shader::uniform_t etm::shader::Texture::getModel() const {
    return model;
}
etm::shader::uniform_t etm::shader::Texture::getColor() const {
    throw fe_error(
    "etm::shader::Texture::getColor: "
    "Texture shader does not have a color uniform!\n"
    "Texture shader has been erroniously set.\n"
    "This is a bug.");
}

