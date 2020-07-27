
/**
* Basic Etermal example using the built
* in ETerminal and EShell implementations.
* This particular example uses glfw.
*/

#include <etermal/etermal.h>
#include <iostream>
#include <chrono>
#include <thread>

// Re-using the GLAD files used by Etermal itself -
// you can generate your own.
#define GLFW_DLL 1
#include "../extern/glad/glad.h"
#include <glfw/glfw3.h>

static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseClick(GLFWwindow* window, int button, int action, int mods);
static void mouseMove(GLFWwindow* window, double xpos, double ypos);
static void charCallback(GLFWwindow* window, unsigned int codepoint);
static void error_callback(int error, const char* description);

// This is only for demonstrative purposes - 
// obviously, you won't actually have it global.
static etm::Terminal terminal(
    "C:\\Windows\\Fonts\\lucon.ttf", // Font to use
    true // `true` to postpone initialization (because there's no active OpenGL context)
);

int main() {

    // Just in case
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "Failed to initialze GLFW" << std::endl;
        return 1;
    }
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

    // Set all the callbacks
    glfwSetScrollCallback(window, mouseScroll);
    glfwSetKeyCallback(window, keyPress);
    glfwSetMouseButtonCallback(window, mouseClick);
    glfwSetCursorPosCallback(window, mouseMove);
    glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetCharCallback(window, charCallback);

    // Cursor for terminal hover
    GLFWcursor* ibeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

    terminal.setWidth(700);
    terminal.setHeight(700);
    terminal.setScrollCooldown(75);
    terminal.setScrollWait(200);
    terminal.setCursorDefault([window]()->void{
        glfwSetCursor(window, NULL);
    });
    terminal.setCursorIBeam([window, ibeam]()->void{
        glfwSetCursor(window, ibeam);
    });
    terminal.setMaxLines(200);

    std::ostream stream(&terminal);

    etm::Shell shell;

    shell.setPrompt("\n\e[B\e[f199e3d;example@\e[b4c1b4f;some terminal somewhere\e[B \e[fb8b816;~\e[F\n$ ");

    terminal.setShell(shell);
    shell.setTerminal(terminal);

    etm::ArgFilter clearFilter;
    clearFilter.setUsage("Usage: clear\n");
    clearFilter.setMaxArrayArgs(0);
    shell.addCommand("clear", clearFilter, [](etm::Shell&,etm::ETerminal &terminal,etm::Args&)->void{
        terminal.clear();
    });

    etm::ArgFilter exitFilter;
    exitFilter.setMaxArrayArgs(0);
    exitFilter.setUsage("Usage: [exit|leave|stop|terminate|cya|bye]\n");
    shell.addCommand("exit", exitFilter, [window](etm::Shell&,etm::ETerminal &terminal,etm::Args&)->void{
        glfwSetWindowShouldClose(window, true);
    });
    shell.alias("leave");
    shell.alias("stop");
    shell.alias("terminate");
    shell.alias("cya");
    shell.alias("bye");

    etm::ArgFilter echoFilter;
    echoFilter.setUsage("Usage: echo [string]\n");
    echoFilter.setMaxArrayArgs(10000);
    shell.addCommand("echo", echoFilter, [&stream](etm::Shell&,etm::ETerminal&,etm::Args &args)->void{
        for (std::string &s : args.getArrayArgs()) {
            stream << s << " ";
        }
        stream << "\n";
    });

    /////////////////

    // Make extra sure that the window is current
    // before initializing the terminal!
    glfwMakeContextCurrent(window);

    // Since we set late initialization to true,
    // MUST call init before render!
    terminal.init();

    /////////////////

    // 60 FPS
    constexpr float secondsPerFrame = 1.0f / 60.0f;
    float endTime = 0;

    while (!glfwWindowShouldClose(window)) {


        // Don't render until frame cooldown is done
        if (glfwGetTime() >= endTime) {
            endTime = glfwGetTime() + secondsPerFrame;
            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Render the terminal
            terminal.render();

            glfwSwapBuffers(window);
        } else {
            // Sleep a little so that our CPU usage isn't super high
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        glfwPollEvents();
    }

    // Free OpenGL resources before we terminate the context
    terminal.deInit();
    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}

void mouseScroll(GLFWwindow* window, double xoffset, double yoffset) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
    terminal.inputMouseScroll(
        static_cast<float>(yoffset),
        static_cast<float>(mouseX),
        static_cast<float>(mouseY)
    );
}

void keyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) return;

    if (terminal.isFocused()) {
        switch (key) {
            case GLFW_KEY_ENTER:
                terminal.inputActionKey(etm::actionKey::ENTER);
                break;
            case GLFW_KEY_BACKSPACE:
                terminal.inputActionKey(etm::actionKey::BACKSPACE);
                break;
            case GLFW_KEY_UP:
                terminal.inputActionKey(etm::actionKey::UP);
                break;
            case GLFW_KEY_DOWN:
                terminal.inputActionKey(etm::actionKey::DOWN);
                break;
            case GLFW_KEY_LEFT:
                terminal.inputActionKey(etm::actionKey::LEFT);
                break;
            case GLFW_KEY_RIGHT:
                terminal.inputActionKey(etm::actionKey::RIGHT);
                break;
            case GLFW_KEY_C:
                if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
                    glfwSetClipboardString(window, terminal.getTextSelection().c_str());
                }
                break;
            case GLFW_KEY_V:
                if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
                    terminal.inputString(glfwGetClipboardString(window));
                }
                break;
        }
    }
}

static void charCallback(GLFWwindow* window, unsigned int codepoint) {
    terminal.inputChar(static_cast<char>(codepoint));
}

void mouseClick(GLFWwindow* window, int button, int action, int mods) {
    double mouseX, mouseY;
    glfwGetCursorPos(glfwGetCurrentContext(), &mouseX, &mouseY);
    terminal.inputMouseClick(
        action == GLFW_PRESS,
        static_cast<float>(mouseX),
        static_cast<float>(mouseY)
    );
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    terminal.inputMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
}

void error_callback(int error, const char* description) {
    std::cerr << "GLFW ERROR: " << description << std::endl;
}
