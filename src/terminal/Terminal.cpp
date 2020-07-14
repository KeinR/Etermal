#include "Terminal.h"

#include <iostream>

#include "render/glfw.h"
#include "util/util.h" // TEMP
#include "../shell/TermInput.h"
#include "../shell/EShell.h"

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    display(resources->getFont(), 30),
    background(resources.get()),
    focused(true),
    takeInput(false),
    escapeNext(false),
    cursorBlink(500),
    shell(nullptr)
{
    updatePosition();

    resources->getFont().setSize(18);
    // std::cout << "widdasd = " << (400 / (resources->getFont().getFace()->size->metrics.max_advance / 64)) << std::endl;

    display.setWidth(400 / (resources->getFont().getFace()->size->metrics.max_advance / 64));

    // output.getElements().push_back(&text);
    // output.update();

    // text.setColor(0xffffff);
    // text.setFontSize(19);

    // TEMP
    // dispText(
    //     // "f"
    //     "fell like a rain AA gg kk qq iii"
    //     // "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n"
    //     // "llllllllllllllllllllllllllllllll\n"
    //     // "ABCDEFGHIJKLMNOPQRSTUVWXYZZZZZZZ\n"
    //     // "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWw"
    // );
    // flush();

    FT_Face f = resources->getFont().getFace();

    std::cout << "max descender = " << (f->size->metrics.descender / 64.0f) << std::endl;

    // float height = (f->ascender - f->descender) / 64.0f;
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
    display.setCursorEnabled(false);
    // displayPrompt();
    // prepareInput();
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
    if (inputBuffer.size()) {
        if (inputRequests.size()) {
            inputRequests.front()->terminalInput(inputBuffer);
            inputRequests.pop_front();
        } else {
            // Print the default prompt, etc.
            std::cout << "SENDING TO SHELL: " << inputBuffer << std::endl;
            shell->input(inputBuffer);
        }
        inputBuffer.clear();
    }
    if (acceptInput()) {
        prepareInput();
    }
}

void etm::Terminal::deleteLastChar() {
    // TODO
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
            deleteLastChar();
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
void etm::Terminal::inputMouseScroll(float yOffset) {

}
void etm::Terminal::inputMouseClick(bool isPressed, float mouseX, float mouseY) {
    // focused = viewport.hasPoint();
}
void etm::Terminal::inputMouseMove(float mouseX, float mouseY) {

}

void etm::Terminal::render() {
    // Store caller's state
    GLboolean scissor, blend;
    GLint program;
    GLint srcRGB, dstRGB, srcAlpha, dstAlpha;
    glGetBooleanv(GL_SCISSOR_TEST, &scissor);
    glGetBooleanv(GL_BLEND, &blend);
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    // ty stack https://stackoverflow.com/a/40025210/10821333
    glGetIntegerv(GL_BLEND_SRC_RGB, &srcRGB);
    glGetIntegerv(GL_BLEND_DST_RGB, &dstRGB);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);


    // Apply _our_ desired state
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    assertGLErr("Terminal.cpp:94");
    display.render(resources.get());
    assertGLErr("Terminal.cpp:96");



    // Restore caller's state
    if (!scissor) {
        glDisable(GL_SCISSOR_TEST);
    }
    if (!blend) {
        glDisable(GL_BLEND);
    }
    glUseProgram(program);
    glBlendFuncSeparate(srcRGB, dstAlpha, srcAlpha, dstAlpha);
}
