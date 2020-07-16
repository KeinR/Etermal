#include "TextState.h"

etm::tm::TextState::TextState(shader::Shader &shader, const Color &defBackgroundColor, const Color &defForegroundColor):
    shader(&shader), defBackgroundColor(&defBackgroundColor), defForegroundColor(&defForegroundColor)
{
    setBack(getDefBack());
    setFore(getDefFore());
}

etm::shader::Shader &etm::tm::TextState::getShader() {
    return *shader;
}

const etm::Color &etm::tm::TextState::getBack() {
    return *backgroundColor;   
}
const etm::Color &etm::tm::TextState::getDefBack() {
    return *defBackgroundColor;
}
const etm::Color &etm::tm::TextState::getFore() {
    return *foregroundColor;   
}
const etm::Color &etm::tm::TextState::getDefFore() {
    return *defForegroundColor;
}

void etm::tm::TextState::setBack(const Color &color) {
    if (color != *backgroundColor) {
        backgroundColor = &color;
        backgroundColor->setBackground(*shader);
    }
}
void etm::tm::TextState::setFore(const Color &color) {
    if (color != *foregroundColor) {
        foregroundColor = &color;
        foregroundColor->setForeground(*shader);
    }
}
