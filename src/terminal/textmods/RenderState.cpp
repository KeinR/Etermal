#include "RenderState.h"

#include "../render/Color.h"

etm::tm::RenderState::RenderState(shader::Shader &shaderP, const Color &defBackgroundColorP, const Color &defForegroundColorP):
    shader(&shaderP),
    backgroundColor(&defBackgroundColorP),
    defBackgroundColor(&defBackgroundColorP),
    foregroundColor(&defForegroundColorP),
    defForegroundColor(&defForegroundColorP)
{
    backgroundColor->setBackground(shaderP);
    foregroundColor->setForeground(shaderP);
}

void etm::tm::RenderState::setDefBack() {
    setBack(*defBackgroundColor);
}
void etm::tm::RenderState::setDefFore() {
    setFore(*defForegroundColor);
}

void etm::tm::RenderState::setBack(const Color &color) {
    backgroundColor = &color;
    backgroundColor->setBackground(*shader);
}
void etm::tm::RenderState::setFore(const Color &color) {
    foregroundColor = &color;
    foregroundColor->setForeground(*shader);
}
