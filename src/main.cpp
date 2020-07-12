#include <iostream>

#include "../src/terminal/Terminal.h"
#include "../src/terminal/util/util.h"

#define GLFW_DLL
#include <glad/glad.h>
#include <glfw/glfw3.h>

static etm::Terminal *terminal;

static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseClick(GLFWwindow* window, int button, int action, int mods);
static void mouseMove(GLFWwindow* window, double xpos, double ypos);

int main() {
    try {
        glfwInit();
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow *window = glfwCreateWindow(700, 700, "Terminal test", NULL, NULL);
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

        etm::assertGLErr("Initialization proc");

        std::cout << "loop" << std::endl;

        while (!glfwWindowShouldClose(window)) {

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            terminal->render();

            etm::assertGLErr("Render loop");

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
