#include "Text.h"

#include "../render/opengl.h"
#include "../Resources.h"

#define MODEL "Model"
#define BACKGROUND_COLOR "backgroundColor"
#define FOREGROUND_COLOR "foregroundColor"
#define SAMPLER0 "Tex"

#include "../../../resources/shaders/text.h"

etm::shader::Text::Text(Resources *res):
    Shader(res, text_vert, text_vert_len, text_frag, text_frag_len),
    res(res),
    backgroundColor(glGetUniformLocation(get(), BACKGROUND_COLOR)),
    foregroundColor(glGetUniformLocation(get(), FOREGROUND_COLOR)),
    model(glGetUniformLocation(get(), MODEL))
{
    use();
    glUniform1i(glGetUniformLocation(get(), SAMPLER0), 0);
}
etm::shader::uniform_t etm::shader::Text::getModel() const {
    return model;
}
etm::shader::uniform_t etm::shader::Text::getColor() const {
    res->postError(
        "etm::shader::Text::getColor()",
        "Text shader does not have a color uniform!\n"
        "Text shader has been erroniously set.\n"
        "This is a bug.",
        0,
        true
    );
    return -1;
}

etm::shader::uniform_t etm::shader::Text::getBackGColor() const {
    return backgroundColor;
}
etm::shader::uniform_t etm::shader::Text::getForeGColor() const {
    return foregroundColor;
}

