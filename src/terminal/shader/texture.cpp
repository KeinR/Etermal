#include "Texture.h"

#include "../render/opengl.h"
#include "../Resources.h"

// Names of uniforms (for lookup)
static const char *MODEL = "Model";
static const char *SAMPLER0 = "Tex";

// Shaders compiled into a single header to enable embedding
#include "../../../resources/shaders/texture.h"

etm::shader::Texture::Texture(Resources *res):
    Shader(res, texture_vert, texture_vert_len, texture_frag, texture_frag_len),
    res(res),
    model(glGetUniformLocation(get(), MODEL))
{
    use();
    glUniform1i(glGetUniformLocation(get(), SAMPLER0), 0);
}
etm::shader::uniform_t etm::shader::Texture::getModel() const {
    return model;
}
etm::shader::uniform_t etm::shader::Texture::getColor() const {
    res->postError(
        "etm::shader::Texture::getColor()",
        "Texture shader does not have a color uniform!\n"
        "Texture shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}

etm::shader::uniform_t etm::shader::Texture::getBackGColor() const {
    res->postError(
        "etm::shader::Texture::getBackGColor()",
        "Texture shader does not have a background color uniform!\n"
        "Texture shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}
etm::shader::uniform_t etm::shader::Texture::getForeGColor() const {
    res->postError(
        "etm::shader::Texture::getForeGColor()",
        "Texture shader does not have a foreground color uniform!\n"
        "Texture shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}

