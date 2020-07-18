#include <iostream>
#include <chrono>

#include "../src/shell/Shell.h"

#include "../src/terminal/Terminal.h"
#include "../src/terminal/util/util.h"
#include "../src/terminal/util/enums.h"

#define GLFW_DLL
#include <glad/glad.h>
#include <glfw/glfw3.h>

static etm::Terminal *terminal;

static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseClick(GLFWwindow* window, int button, int action, int mods);
static void mouseMove(GLFWwindow* window, double xpos, double ypos);
static void charCallback(GLFWwindow* window, unsigned int codepoint);

int main() {
    try {
        glfwInit();
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_SAMPLES, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow *window = glfwCreateWindow(1000, 700, "Terminal test", NULL, NULL);
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
        glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        glfwSetCharCallback(window, charCallback);

        std::cout << "afs" << std::endl;

        etm::Terminal term;
        terminal = &term;

        GLFWcursor* ibeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

        term.setMaxWidth(700);
        term.setMaxHeight(700);
        term.setCursorDefault([window]()->void{
            glfwSetCursor(window, NULL);
        });
        term.setCursorIBeam([window, ibeam]()->void{
            glfwSetCursor(window, ibeam);
        });

        etm::Shell shell;

        term.setShell(shell);
        shell.setTerminal(term);

        std::cout << "created" << std::endl;

        etm::assertGLErr("Initialization proc");

        std::cout << "loop" << std::endl;

        long samples = 0;
        long totalMillis = 0;
        while (!glfwWindowShouldClose(window)) {

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            auto fstart = std::chrono::high_resolution_clock::now();
            terminal->render();
            totalMillis += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - fstart).count();
            samples++;
            if (samples > 100) {
                std::cout << "LAST FRAME UTILIZATION AVG: " << (static_cast<float>(totalMillis) / samples) << ", " << (static_cast<float>(totalMillis) / samples / (1e6 / 60.0f) * 100) << "% of frame" << std::endl;
                samples = 0;
                totalMillis = 0;
            }

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
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
    terminal->inputMouseScroll(
        static_cast<float>(yoffset),
        static_cast<float>(mouseX),
        static_cast<float>(mouseY)
    );
}

void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) return;

    switch (key) {
        case GLFW_KEY_ENTER:
            terminal->inputActionKey(etm::actionKey::ENTER);
            break;
        case GLFW_KEY_BACKSPACE:
            terminal->inputActionKey(etm::actionKey::BACKSPACE);
            break;
        case GLFW_KEY_UP:
            terminal->inputActionKey(etm::actionKey::UP);
            break;
        case GLFW_KEY_DOWN:
            terminal->inputActionKey(etm::actionKey::DOWN);
            break;
        case GLFW_KEY_LEFT:
            terminal->inputActionKey(etm::actionKey::LEFT);
            break;
        case GLFW_KEY_RIGHT:
            terminal->inputActionKey(etm::actionKey::RIGHT);
            break;
    }
}

static void charCallback(GLFWwindow* window, unsigned int codepoint) {
    terminal->inputChar(static_cast<char>(codepoint));
}

void mouseClick(GLFWwindow* window, int button, int action, int mods) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
    terminal->inputMouseClick(
        action == GLFW_PRESS,
        static_cast<float>(mouseX),
        static_cast<float>(mouseY)
    );
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    terminal->inputMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
}
