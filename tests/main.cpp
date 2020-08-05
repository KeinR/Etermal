#include <iostream>
#include <chrono>
#include <thread>
#include <memory>

#include <etermal/etermal.h>
#include <etermal/include/terminal/util/debug.h>
#include <etermal/include/terminal/util/enums.h>

#define GLFW_DLL 1
#include <glad/glad.h>
#include <glfw/glfw3.h>

static etm::Terminal *terminal;

static void mouseScroll(GLFWwindow* window, double xoffset, double yoffset);
static void keyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouseClick(GLFWwindow* window, int button, int action, int mods);
static void mouseMove(GLFWwindow* window, double xpos, double ypos);
static void charCallback(GLFWwindow* window, unsigned int codepoint);

void error_callback(int error, const char* description) {
    std::cerr << "GLFW ERROR: " << description << std::endl;
}

int main() {
    try {
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

        glfwSetScrollCallback(window, mouseScroll);
        glfwSetKeyCallback(window, keyPress);
        glfwSetMouseButtonCallback(window, mouseClick);
        glfwSetCursorPosCallback(window, mouseMove);
        glfwSetInputMode(window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        glfwSetCharCallback(window, charCallback);

        std::cout << "afs" << std::endl;

        // std::shared_ptr<etm::Font> font = std::make_shared<etm::Font>("C:\\Windows\\Fonts\\lucon.ttf");
        std::shared_ptr<etm::BmpFont> font = std::make_shared<etm::BmpFont>(
            "lucon_aa.bmp",
            32,
            11,
            18,
            160
        );

        etm::Terminal term(true);
        term.setFont(font);

        // {
        //     etm::Terminal othe;
        //     term = std::move(othe);
        // }

        terminal = &term;

        std::cout << "tjgf" << std::endl;

        GLFWcursor* ibeam = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

        term.setWidth(700);
        term.setHeight(700);
        // term.setRows(10, 5);
        term.setScrollCooldown(75);
        term.setScrollWait(200);
        term.setCursorDefault([window]()->void{
            glfwSetCursor(window, NULL);
        });
        term.setCursorIBeam([window, ibeam]()->void{
            glfwSetCursor(window, ibeam);
        });
        term.setMaxLines(200);

        etm::BasicShell shell;

        shell.setPrompt("\n\e[B\e[f199e3d;tester@\e[b4c1b4f;testTerminal\e[B \e[fb8b816;~\e[F\n$ ");

        term.setShell(shell);
        shell.setTerminal(term);

        typedef etm::Shell::comid_t comid;

        // etm::ArgFilter clearFilter;
        // clearFilter.setUsage("Usage: clear\n");
        // clearFilter.setMaxArrayArgs(0);
        // comid clearCom = shell.addCommand("clear", clearFilter, [](etm::Shell&,etm::ETerminal &terminal,etm::Args&)->void{
        //     terminal.clear();
        // });
        // etm::ArgFilter exitFilter;
        // exitFilter.setMaxArrayArgs(0);
        // exitFilter.setUsage("Usage: [exit|leave|stop|terminate|cya|bye]\n");
        // comid exitCom = shell.addCommand("exit", exitFilter, [window](etm::Shell&,etm::ETerminal &terminal,etm::Args&)->void{
        //     glfwSetWindowShouldClose(window, true);
        // });
        // shell.alias(exitCom, "leave");
        // shell.alias(exitCom, "stop");
        // shell.alias(exitCom, "terminate");
        // shell.alias(exitCom, "cya");
        // shell.alias(exitCom, "bye");
        shell.add("clear", [terminal](etm::BasicShell::param_t)->void{
            terminal->clear();
        });

        std::cout << "created" << std::endl;

        etm::assertGLErr("Initialization proc");

        std::cout << "loop" << std::endl;

        std::ostream out(terminal);
        out << "\nhey look I can write to the temrinal!" << std::flush;

        /////////////////

        term.init();

        /////////////////

        long samples = 0;
        long totalMillis = 0;
        etm::Timer timer(1000 / 60);
        while (!glfwWindowShouldClose(window)) {

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            typedef std::chrono::high_resolution_clock clock;

            auto fstart = clock::now();
            terminal->render();
            totalMillis += std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - fstart).count();
            samples++;
            if (samples > 1) {
                // std::cout << "FPS = " << (static_cast<float>(samples) / (static_cast<float>(totalMillis) / 1000.0f)) << ", LAST FRAME UTILIZATION AVG: " << (static_cast<float>(totalMillis) / samples) << ", " << (static_cast<float>(totalMillis) / samples / (1000 / 60.0f) * 100) << "% of frame" << std::endl;
                samples = 0;
                totalMillis = 0;
            }

            etm::assertGLErr("Render loop");

            glfwSwapBuffers(window);
            glfwPollEvents();

            // while (!timer.hasEnded()) {
            //     std::this_thread::sleep_for(std::chrono::milliseconds(1));
            //     glfwPollEvents();
            // }
            // timer.start();
        }

        term.deInit();

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

void keyPress(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) return;

    if (terminal->isFocused()) {
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
            case GLFW_KEY_C:
                if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
                    glfwSetClipboardString(window, terminal->getTextSelection().c_str());
                }
                break;
            case GLFW_KEY_V:
                if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL) {
                    terminal->inputString(glfwGetClipboardString(window));
                }
                break;
        }
    }
}

static void charCallback(GLFWwindow* window, unsigned int codepoint) {
    terminal->inputChar(codepoint);
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
