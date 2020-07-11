#include "util.h"

#include "../render/glfw.h"

void etm::setScissor(int x, int y, int width, int height) {
    int winWidth, winHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &winWidth, &winHeight); // TEMP
    glScissor(
        x,
        winHeight - height - y,
        width,
        height
    );
}

void etm::resetScissor() {
    int winWidth, winHeight;
    glfwGetWindowSize(glfwGetCurrentContext(), &winWidth, &winHeight); // TEMP
    glScissor(
        0,
        0,
        winWidth,
        winHeight
    );
}

