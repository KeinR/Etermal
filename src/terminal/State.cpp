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
    // We only use the first texture slot
    glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
}
void etm::State::set() {
    // Don't set if we don't have to
    if (depth == GL_TRUE) {
        glDisable(GL_DEPTH_TEST);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    if (activeTexture != GL_TEXTURE0) {
        glActiveTexture(GL_TEXTURE0);
    }
}
void etm::State::restore() {
    if (depth == GL_TRUE) {
        glEnable(GL_DEPTH_TEST);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
    }
    if (activeTexture != GL_TEXTURE0) {
        glActiveTexture(activeTexture);
    }
    glUseProgram(program);
}
