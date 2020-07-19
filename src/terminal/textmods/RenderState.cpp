#include "RenderState.h"

#include "../render/Color.h"

etm::tm::RenderState::RenderState(shader::Shader &shaderP, const Color &defBackgroundColorP, const Color &defForegroundColorP, bool startInverted):
    shader(&shaderP),
    backgroundColor(&defBackgroundColorP),
    defBackgroundColor(&defBackgroundColorP),
    foregroundColor(&defForegroundColorP),
    defForegroundColor(&defForegroundColorP),
    inverted(false)
{
    backgroundColor->setBackground(*shader);
    foregroundColor->setForeground(*shader);
    setInverted(startInverted);
}

void etm::tm::RenderState::setDefBack() {
    setBack(*defBackgroundColor);
}
void etm::tm::RenderState::setDefFore() {
    setFore(*defForegroundColor);
}

void etm::tm::RenderState::doSetBack(const Color &color) {
    backgroundColor = &color;
    backgroundColor->setBackground(*shader);
}
void etm::tm::RenderState::doSetFore(const Color &color) {
    foregroundColor = &color;
    foregroundColor->setForeground(*shader);
}
void etm::tm::RenderState::setBack(const Color &color) {
    if (!inverted) {
        doSetBack(color);
    } else {
        doSetFore(color);
    }
}
void etm::tm::RenderState::setFore(const Color &color) {
    if (!inverted) {
        doSetFore(color);
    } else {
        doSetBack(color);
    }
}

void etm::tm::RenderState::setInverted(bool val) {
    if (inverted != val) {
        inverted = val;
        const Color *tmp = backgroundColor;
        doSetBack(*foregroundColor);
        doSetFore(*tmp);
    }
}
