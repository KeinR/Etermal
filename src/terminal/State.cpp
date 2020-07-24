#include "State.h"

#include "render/Model.h"

etm::State::State() {
    store();
}
etm::State::~State() {
    restore();
}
void etm::State::store() {
    // All have the same z, so no depth test
    glGetBooleanv(GL_DEPTH_TEST, &depth);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    // Glyph widths can vary quite a lot
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlign);
}
void etm::State::set() {
    // Don't set if we don't have to
    if (depth) {
        glDisable(GL_DEPTH_TEST);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
}
void etm::State::restore() {
    if (depth) {
        glEnable(GL_DEPTH_TEST);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
    }
    glUseProgram(program);
}
