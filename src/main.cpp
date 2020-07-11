#include <iostream>

#include "../src/terminal/Terminal.h"

#define GLFW_DLL
#include <glad/glad.h>
#include <glfw/glfw3.h>

static etm::Terminal *terminal;

static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseClick(GLFWwindow* window, int button, int action, int mods);
static void mouseMove(GLFWwindow* window, double xpos, double ypos);
static const char *getGLErrorStr(GLenum error);

int main() {
    try {
        glfwInit();
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow *window = glfwCreateWindow(300, 300, "Terminal test", NULL, NULL);
        if (window == NULL) {
            std::cerr << "Failed to make GLFW window" << std::endl;
            return 1;
        }
        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return 1;
        }

        glfwSetScrollCallback(window, mouseScroll);
        glfwSetKeyCallback(window, keyPress);
        glfwSetMouseButtonCallback(window, mouseClick);
        glfwSetCursorPosCallback(window, mouseMove);

        std::cout << "afs" << std::endl;

        etm::Terminal term;
        terminal = &term;

        std::cout << "created" << std::endl;

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "INIT ERROR: " << getGLErrorStr(error) << std::endl;
            return 1;
        }

        std::cout << "loop" << std::endl;

        while (!glfwWindowShouldClose(window)) {

            std::cout << "run loop" << std::endl;

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

            terminal->render();

            GLenum error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cerr << "RENDER ERROR: " << getGLErrorStr(error) << std::endl;
                return 1;
            }

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwTerminate();

        return 0;
    } catch (std::exception &e) {
        std::cout << "ERROR: " << e.what() << std::endl;
    }
    return 1;
}

void mouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    terminal->userScroll(static_cast<float>(yoffset));
}

void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    terminal->userKeyPress('d');
}

void mouseClick(GLFWwindow* window, int button, int action, int mods) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
    terminal->userClick(
        action == GLFW_PRESS,
        static_cast<float>(mouseX),
        static_cast<float>(mouseY)
    );
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    terminal->userMove(static_cast<float>(xpos), static_cast<float>(ypos));
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
