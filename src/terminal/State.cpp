#include "State.h"

#include "render/Model.h"

etm::State::State() {
    glGetBooleanv(GL_SCISSOR_TEST, &scissor);
    glGetBooleanv(GL_BLEND, &blend);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glGetIntegerv(GL_SCISSOR_BOX, scissorCoords);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlign);
}
void etm::State::set(const Model &viewport) {
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int winHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), NULL, &winHeight);
    glScissor(viewport.x, winHeight - viewport.y - viewport.height, viewport.width, viewport.height);
}
void etm::State::restore() {
    if (!scissor) {
        glDisable(GL_SCISSOR_TEST);
    }
    if (blend) {
        glEnable(GL_BLEND);
    }
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
    }
    glUseProgram(program);
    glScissor(scissorCoords[0], scissorCoords[1], scissorCoords[2], scissorCoords[3]);
}
