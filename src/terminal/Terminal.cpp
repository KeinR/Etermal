#include "Terminal.h"

#include <iostream>
#include <cmath>

#include "util/termError.h"
#include "render/glfw.h"
#include "util/debug.h"
#include "../TermInput.h"
#include "../EShell.h"
#include "State.h"
#include "env.h"

#include "textmods/Mod.h"
#include "textmods/mods.h"

static constexpr int BOTTOM_MARGIN = 7;


static void defaultErrorCallback(const etm::termError &error);

void defaultErrorCallback(const etm::termError &error) {
    std::cerr
        << "---------------------------\n"
        << "ETERMAL::TERMINAL::ERROR:\n"
        << "location = " << error.location << "\n"
        << "code = 0x" << std::hex << error.code << std::dec << " (dec = " << error.code << ")" << "\n"
        << "severe = " << (error.severe ? "TRUE" : "FALSE") << "\n"
        << "message = \"" << error.message << "\"\n"
        << "---------------------------\n";
}

etm::Terminal::Terminal(): Terminal(defaultErrorCallback) {
}
etm::Terminal::Terminal(const errCallback_t &errorCallback):
    // Make sure that the provided callback is valid
    errorCallback(errorCallback ? errorCallback : defaultErrorCallback),
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    scrollbar(resources.get(), scroll),
    scrollSensitivity(25.0f),
    display(resources.get(), scroll, 30),
    background(resources.get()),
    focused(true),
    takeInput(false),
    escapeNext(false),
    cursorBlink(500),
    shell(nullptr)
{
    setBackgroundColor(0x0f0f0f);
    setTextColor(0xffffff);

    scrollbar.setWidth(22);
    scrollbar.setSideMargin(2);
    scrollbar.setSliderColor(0xbababa);
    scrollbar.setBarColor(0xf5f5f5);

    setFontSize(18);

    // Superfluous given that setFontSize() calls it, however just
    // to make sure...
    updatePosition();

    displayWelcome();
    display.setCursorEnabled(false);
}

void etm::Terminal::displayWelcome() {
    dispText("Etermal v0.1\n");
    flush();
}

void etm::Terminal::prepareInput() {
    display.jumpCursor();
    display.lockCursor();
    display.setCursorEnabled(true);
}

bool etm::Terminal::acceptInput() {
    return takeInput || inputRequests.size();
}

void etm::Terminal::flushInputBuffer() {
    display.setCursorEnabled(false);
    if (inputRequests.size()) {
        inputRequests.front()->terminalInput(inputBuffer);
        inputRequests.pop_front();
    } else if (shell != nullptr) {
        // Print the default prompt, etc.
        std::cout << "SENDING TO SHELL: " << inputBuffer << std::endl;
        shell->input(inputBuffer);
    } else {
        resources->postError(
            "Terminal::flushInputBuffer()",
            "Shell is nullptr (not set)",
            0,
            false
        );
    }
    inputBuffer.clear();
    if (acceptInput()) {
        prepareInput();
    }
}

void etm::Terminal::postError(const termError &error) {
    errorCallback(error);
}

void etm::Terminal::setCursorDefault(const winActionCB_t &callback) {
    windowSetCursorDefault = callback;
}
void etm::Terminal::setCursorIBeam(const winActionCB_t &callback) {
    windowSetCursorIBeam = callback;
}
void etm::Terminal::setHovering(bool value) {
    if (hovering != value) {
        hovering = value;
        if (hovering) {
            if (windowSetCursorIBeam) {
                windowSetCursorIBeam();
            }
        } else if (windowSetCursorDefault) {
            windowSetCursorDefault();
        }
    }
}
void etm::Terminal::setErrorCallback(const errCallback_t &callback) {
    if (callback) {
        errorCallback = callback;
    }
}

void etm::Terminal::clear() {
    display.clear();
}

void etm::Terminal::setBackgroundColor(const Color &color) {
    background.setColor(color);
    display.setDefBackGColor(color);
}
void etm::Terminal::setTextColor(const Color &color) {
    display.setDefForeGColor(color);
}

void etm::Terminal::setScrollSensitivity(float value) {
    scrollSensitivity = value;
}

// The shell is where user input will be directed
void etm::Terminal::setShell(EShell &shell) {
    this->shell = &shell;
}

void etm::Terminal::setTakeInput(bool value) {
    if (takeInput != value) {
        if (!takeInput && !inputRequests.size()) {
            prepareInput();
        }
        takeInput = value;
    }
}

void etm::Terminal::requestInput(TermInput &callback) {
    if (!acceptInput()) {
        prepareInput();
    }
    inputRequests.push_back(&callback);
}
void etm::Terminal::cancelInputRequest(TermInput *callback) {
    typedef inputRequests_t::iterator it_t;
    for (it_t it = inputRequests.begin(); it < inputRequests.end();) {
        if (*it == callback) {
            it = inputRequests.erase(it);
        } else {
            ++it;
        }
    }
}
void etm::Terminal::clearInputRequests() {
    inputRequests.clear();
}

void etm::Terminal::dispText(const std::string &str) {
    displayBuffer += str;
}

int etm::Terminal::readHexFromStr(std::string &str, std::string::size_type &i) {
    i++;
    int result = 0;
    for (std::string::size_type end = i + 6; i < str.size() && i < end && str[i] != ';'; i++) {
        if ('0' <= str[i] && str[i] <= '9') {
            result = (result * 16) + str[i] - '0';
        } else {
            const char c = str[i] | 0x20;
            if ('a' <= c && c <= 'f') {
                result = (result * 16) + c - 'a' + 10;
            } else {
                break;
            }
        }
    }
    std::cout << "read hex 0x" << std::hex << result << std::dec << std::endl;
    return result;
}

void etm::Terminal::flush() {
    constexpr char ESCAPE = '\x1b';
    for (std::string::size_type i = 0; i < displayBuffer.size(); i++) {
        // Escape char
                                        // 1 for the [, 1 for the spec (b/f)
        if (displayBuffer[i] == ESCAPE && i + 2 < displayBuffer.size() && displayBuffer[i+1] == '[') {
            std::string::size_type fi = i + 2;
            std::shared_ptr<tm::Mod> mod;
            switch (displayBuffer[fi]) {
                case 'b':
                    mod = std::make_shared<tm::Background>(readHexFromStr(displayBuffer, fi));
                    break;
                case 'f':
                    mod = std::make_shared<tm::Foreground>(readHexFromStr(displayBuffer, fi));
                    break;
                case 'B':
                    mod = std::make_shared<tm::RevBackground>();
                    break;
                case 'F':
                    std::cout << "SET MOD" << std::endl;
                    mod = std::make_shared<tm::RevForeground>();
                    break;
                case 'r':
                    mod = std::make_shared<tm::RevBackFore>();
                    break;
            }
            if (mod) {
                std::cout << "add" << std::endl;
                display.pushMod(mod);
                i = fi;
            } else {
                display.append(displayBuffer[i]);
            }
        } else {
            display.append(displayBuffer[i]);
        }
    }
    displayBuffer.clear();
    const bool jump = scroll.getOffset() + 1 >= scroll.getMaxOffset();
    scroll.setGrossHeight(display.getHeight());
    if (jump) {
        scroll.jump();
    }
    scrollbar.update();
}

void etm::Terminal::setX(float x) {
    viewport.x = x;
    updatePosition();
}
void etm::Terminal::setY(float y) {
    viewport.y = y;
    updatePosition();
}
void etm::Terminal::setMaxWidth(float width) {
    viewport.width = resources->getFont().getCharWidth() * std::floor(width / resources->getFont().getCharWidth());
    updatePosition();
}
void etm::Terminal::setMaxHeight(float height) {
    viewport.height = resources->getFont().getCharHeight() * std::floor((height - BOTTOM_MARGIN) / resources->getFont().getCharHeight());
    viewport.height += BOTTOM_MARGIN;
    updatePosition();
}

void etm::Terminal::setFontSize(unsigned int size) {
    resources->getFont().setSize(size);
    scroll.setAlign(resources->getFont().getCharHeight());
    updatePosition();
}

void etm::Terminal::updatePosition() {
    background.setX(viewport.x);
    background.setY(viewport.y);
    background.setWidth(viewport.width - scrollbar.getWidth());
    background.setHeight(viewport.height);

    scrollbar.setX(viewport.x + viewport.width - scrollbar.getWidth());
    scrollbar.setY(viewport.y);
    scrollbar.setHeight(viewport.height);

    scroll.setNetHeight(viewport.height - BOTTOM_MARGIN);

    display.setWidth(background.getWidth() / resources->getFont().getCharWidth());
}

void etm::Terminal::inputChar(char c) {
    if (acceptInput()) {
        doInputChar(c);
        scroll.jump();
        scrollbar.update();
    }
}
void etm::Terminal::doInputChar(char c) {
    if (!escapeNext) {
        switch (c) {
            case '\n':
                display.jumpCursor(); // Avoid distorted text
                display.insertAtCursor(c);
                flushInputBuffer();
                break;
            case '\\':
                escapeNext = true;
                break;
            default:
                display.insertAtCursor(c);
                inputBuffer.push_back(c);
        }
    } else {
        display.insertAtCursor(c);
        inputBuffer.push_back(c);
    }
}
void etm::Terminal::inputString(const std::string &text) {
    for (char c : text) {
        if (!acceptInput()) break;
        inputChar(c);
    }
}
void etm::Terminal::inputActionKey(actionKey key) {
    switch (key) {
        case ENTER:
            inputChar('\n');
            break;
        case BACKSPACE:
            display.eraseAtCursor();
            break;
        case UP:
            display.moveCursorRow(-1);
            break;
        case DOWN:
            display.moveCursorRow(1);
            break;
        case LEFT:
            display.moveCursorCollumnWrap(-1);
            break;
        case RIGHT:
            display.moveCursorCollumnWrap(1);
            break;
        // Ignore if there's no match
    }
}
void etm::Terminal::inputMouseScroll(float yOffset, float mouseX, float mouseY) {
    if (viewport.hasPoint(mouseX, mouseY)) {
        if (scroll.scroll(-yOffset * scrollSensitivity)) {
            scrollbar.update();
        }
    }
}
void etm::Terminal::inputMouseClick(bool isPressed, float mouseX, float mouseY) {
    scrollbar.mouseClick(isPressed, mouseX, mouseY);
    focused = background.hasPoint(mouseX, mouseY);
}
void etm::Terminal::inputMouseMove(float mouseX, float mouseY) {
    scrollbar.mouseMove(mouseX, mouseY);
    setHovering(background.hasPoint(mouseX, mouseY));
}

void etm::Terminal::render() {
    State state;
    state.store();
    state.set();

    // Run animiations
    if (cursorBlink.hasEnded()) {
        display.toggleCursor();
        cursorBlink.start();
    }

    // Render
    resources->bindPrimitiveShader();
    background.render();
    scrollbar.render();
    // After scrollbar renders, the text shader is set
    // due to the rendering of the triangle textures,
    // so display doesn't need to set it.
    display.render(viewport.x, viewport.y - scroll.getOffset());

    state.restore();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        resources->postError(
            "Terminal::render()",
            std::string("OpenGL error - ") + getGLErrorStr(error),
            error,
            true
        );
    }
}

bool etm::Terminal::isFocused() {
    return focused;
}

void etm::Terminal::setFocused(bool val) {
    focused = val;
}
