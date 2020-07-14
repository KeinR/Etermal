#include "Terminal.h"

#include <iostream>

#include "render/glfw.h"
#include "util/util.h" // TEMP
#include "../shell/TermInput.h"
#include "../shell/EShell.h"
#include "State.h"

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    display(resources.get(), 30),
    background(resources.get()),
    focused(true),
    takeInput(false),
    escapeNext(false),
    scrollbar(resources.get(), scroll),
    cursorBlink(500),
    shell(nullptr)
{
    background.setColor(0x0f0f0f);

    scrollbar.setWidth(20);
    scrollbar.setSideMargin(2);
    scrollbar.setSliderColor(0xbababa);
    scrollbar.setBarColor(0xf5f5f5);

    updatePosition();

    resources->getFont().setSize(18);

    display.setWidth(400 / (resources->getFont().getFace()->size->metrics.max_advance / 64));

    displayWelcome();
    display.setCursorEnabled(false);
}

void etm::Terminal::displayWelcome() {
    dispText("Etermal v0.1\n");
    flush();
}
void etm::Terminal::displayPrompt() {
    dispText("\nuser@terminal ~\n$ ");
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
    } else {
        // Print the default prompt, etc.
        std::cout << "SENDING TO SHELL: " << inputBuffer << std::endl;
        shell->input(inputBuffer);
    }
    inputBuffer.clear();
    if (acceptInput()) {
        prepareInput();
    }
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
void etm::Terminal::flush() {
    for (char c : displayBuffer) {
        display.append(c);
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
}
void etm::Terminal::setY(float y) {
    viewport.y = y;
}
void etm::Terminal::setMaxWidth(float width) {
    viewport.width = width;
}
void etm::Terminal::setMaxHeight(float height) {
    viewport.height = height;
}

void etm::Terminal::updatePosition() {
    background.setX(viewport.x);
    background.setY(viewport.y);
    background.setWidth(viewport.width - scrollbar.getWidth());
    background.setHeight(viewport.height);

    scrollbar.setX(viewport.x + viewport.width - scrollbar.getWidth());
    scrollbar.setY(viewport.y);
    scrollbar.setHeight(viewport.height);

    scroll.setNetHeight(viewport.height);
}

void etm::Terminal::inputChar(char c) {
    if (acceptInput()) {
        doInputChar(c);
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
        scroll.scroll(-yOffset * 6);
        scrollbar.update();
        std::cout << "scrolling" << std::endl;
    }
}
void etm::Terminal::inputMouseClick(bool isPressed, float mouseX, float mouseY) {
    scrollbar.mouseClick(isPressed, mouseX, mouseY);
}
void etm::Terminal::inputMouseMove(float mouseX, float mouseY) {
    scrollbar.mouseMove(mouseX, mouseY);
}

void etm::Terminal::render() {
    State state;
    state.set(viewport);


    // Run animiations
    if (cursorBlink.hasEnded()) {
        display.toggleCursor();
        cursorBlink.start();
    }

    // Render
    assertGLErr("Terminal.cpp:91");
    resources->bindPrimitiveShader();
    assertGLErr("Terminal.cpp:92");
    background.render();
    scrollbar.render();
    assertGLErr("Terminal.cpp:94");
    display.render(viewport.x, viewport.y - scroll.getOffset());
    assertGLErr("Terminal.cpp:96");


    state.restore();
}
