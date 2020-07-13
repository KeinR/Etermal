#include "Terminal.h"

#include <iostream>

#include "render/glfw.h"
#include "util/util.h" // TEMP
#include "TermInput.h"

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    display(resources->getFont(), 30),
    background(resources.get()),
    focused(true),
    takeInput(true),
    escapeNext(false),
    shell(nullptr)
{
    updatePosition();
    // output.getElements().push_back(&text);
    // output.update();

    // text.setColor(0xffffff);
    // text.setFontSize(19);

    // TEMP
    dispText(
        // "f"
        "fell like a rain AA gg kk qq iii"
        // "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n"
        // "llllllllllllllllllllllllllllllll\n"
        // "ABCDEFGHIJKLMNOPQRSTUVWXYZZZZZZZ\n"
        // "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWw"
    );
    flush();

    resources->getFont().setSize(18);
    // FT_Face f = resources->getFont().getFace();

    // // float height = (f->ascender - f->descender) / 64.0f;
    // float height = (f->size->metrics.height) / 64.0f;
    // // float width = (f->max_advance_width) / 64.0f;
    // float width = (f->size->metrics.max_advance) / 64.0f;
    // std::cout << "height = " << height << std::endl;
    // std::cout << "width = " << width << std::endl;
    // std::cout << "left = " << f->bbox.xMax << std::endl;
    // // std::cout << "right = " << width << std::endl;
    // std::cout << "f->max_advance_width = " << f->max_advance_width << std::endl;

    // std::cout << "bounds = " << (resources->getFont().getFace()->bbox.xMax -
    //                             resources->getFont().getFace()->bbox.xMin) / resources->getFont().getFace()->units_per_EM << std::endl;

    displayWelcome();
    displayPrompt();
}

void etm::Terminal::displayWelcome() {
    dispText("Welcome to terminal!\n");
    flush();
}
void etm::Terminal::displayPrompt() {
    dispText("\nuser@terminal ~\n$ ");
    flush();
}

void etm::Terminal::flushInputBuffer() {
    if (inputBuffer.size()) {
        if (inputRequests.size()) {
            inputRequests.front()->terminalInput(inputBuffer);
            inputRequests.pop_front();
        } else {
            // Print the default prompt, etc.
            std::cout << "SENDING TO SHELL: " << inputBuffer << std::endl;
        }
        inputBuffer.clear();
        displayPrompt();
    }
}

void etm::Terminal::deleteLastChar() {
    // TODO
}

// The shell is where user input will be directed
void etm::Terminal::setShell(Shell &shell) {
    this->shell = &shell;
}

void etm::Terminal::setTakeInput(bool value) {
    takeInput = value;
}

void etm::Terminal::requestInput(TermInput &callback) {
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
    // output.setX(viewport.x);
    // output.setY(viewport.y);
    // output.setWidth(viewport.width);
    // output.setHeight(viewport.height - 30);
    background.setX(viewport.x);
    background.setY(viewport.y);
    background.setWidth(viewport.width);
    background.setHeight(viewport.height - 30);
    background.setColor(0x0f0f0f);
}

void etm::Terminal::inputChar(char c) {
    if (takeInput) {
        doInputChar(c);
    }
}
void etm::Terminal::doInputChar(char c) {
    if (!escapeNext) {
        switch (c) {
            case '\n':
                flushInputBuffer();
                display.append(c);
                break;
            case '\\':
                escapeNext = true;
                break;
            default:
                display.append(c);
                inputBuffer.push_back(c);
        }
    } else {
        display.append(c);
        inputBuffer.push_back(c);
    }
}
void etm::Terminal::inputString(const std::string &text) {
    for (char c : text) {
        if (!takeInput) break;
        inputChar(c);
    }
}
void etm::Terminal::inputActionKey(actionKey key) {
    switch (key) {
        case ENTER:
            inputChar('\n');
            break;
        case BACKSPACE:
            deleteLastChar();
            break;
        case UP:
            // TODO
            break;
        case DOWN:
            // TODO
            break;
        case LEFT:
            // TODO
            break;
        case RIGHT:
            // TODO
            break;
        // Ignore if there's no match
    }
}
void etm::Terminal::inputMouseScroll(float yOffset) {

}
void etm::Terminal::inputMouseClick(bool isPressed, float mouseX, float mouseY) {
    // focused = viewport.hasPoint();
}
void etm::Terminal::inputMouseMove(float mouseX, float mouseY) {

}

void etm::Terminal::render() {
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    assertGLErr("Terminal.cpp:91");
    resources->bindPrimitiveShader();
    assertGLErr("Terminal.cpp:92");
    background.render();
    assertGLErr("Terminal.cpp:94");
    resources->bindTextureShader();
    display.render(resources.get());
    assertGLErr("Terminal.cpp:96");
    // output.render();
    // input.render();
    // resources->bindTextureShader();
    // lines[0].render();
    // testText.render();
    // testImage.render();
}
