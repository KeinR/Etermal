#include "State.h"

#include "render/Model.h"

etm::State::State() {
    glGetBooleanv(GL_SCISSOR_TEST, &scissor);
    glGetBooleanv(GL_BLEND, &blend);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glGetIntegerv(GL_SCISSOR_BOX, scissorCoords);
    // ty stack https://stackoverflow.com/a/40025210/10821333
    glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
}
void etm::State::set(const Model &viewport) {
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int winHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), NULL, &winHeight);
    glScissor(viewport.x, winHeight - viewport.y - viewport.height, viewport.width, viewport.height);
}
void etm::State::restore() {
    if (!scissor) {
        glDisable(GL_SCISSOR_TEST);
    }
    if (!blend) {
        glDisable(GL_BLEND);
    }
    glUseProgram(program);
    glScissor(scissorCoords[0], scissorCoords[1], scissorCoords[2], scissorCoords[3]);
    glBlendFuncSeparate(srcRGB, dstAlpha, srcAlpha, dstAlpha);
}
