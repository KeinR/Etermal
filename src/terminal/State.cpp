#include "State.h"

#include "render/Model.h"

etm::State::State() {
    glGetBooleanv(GL_BLEND, &blend);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlign);
}
void etm::State::set(const Model &viewport) {
    if (blend) {
        glDisable(GL_BLEND);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
}
void etm::State::restore() {
    if (blend) {
        glEnable(GL_BLEND);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
    }
    glUseProgram(program);
}
