#include "util.h"

#include <iostream>

#include "../render/glfw.h"

static const char *getGLErrorStr(GLenum error);

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

void etm::assertGLErr(const char *location) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "GL ERROR @[" << location << "]: " << getGLErrorStr(error) << std::endl;
        exit(1);
    }
}

const char *getGLErrorStr(GLenum error) {
    switch (error) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default: return "-Unknown error-";
    }
}

