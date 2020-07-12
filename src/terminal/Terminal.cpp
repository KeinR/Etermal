#include "Terminal.h"

#include <iostream>

#include "render/glfw.h"
#include "util/util.h" // TEMP

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    text(resources.get()),
    output(resources.get()),
    background(resources.get()),
    focused(false),
    shell(nullptr)
{
    updatePosition();
    output.getElements().push_back(&text);
    output.update();

    text.setColor(0xffffff);

    // TEMP
    dispText("fell like a rain");
    flush();

    FT_Face f = resources->getFont().getFace();

    int height = (f->ascender - f->descender) / 64;
    int width = (f->max_advance_width) / 64;
    std::cout << "height = " << height << std::endl;
    std::cout << "width = " << width << std::endl;
    std::cout << "f->max_advance_width = " << f->max_advance_width << std::endl;

    // std::cout << "bounds = " << (resources->getFont().getFace()->bbox.xMax -
    //                             resources->getFont().getFace()->bbox.xMin) / resources->getFont().getFace()->units_per_EM << std::endl;
}

// The shell is where user input will be directed
void etm::Terminal::setShell(Shell &shell) {
    this->shell = &shell;
}

void etm::Terminal::dispText(const std::string &str) {
    textBuffer += str;
}

void etm::Terminal::flush() {
    text.setString(textBuffer);
    text.generate();
}

void etm::Terminal::inputText(const std::string &str) {
    // Send info to shell
    std::cout << "input ------->> " << str << std::endl;
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
    output.setX(viewport.x);
    output.setY(viewport.y);
    output.setWidth(viewport.width);
    output.setHeight(viewport.height - 30);
    background.setX(viewport.x);
    background.setY(viewport.y);
    background.setWidth(viewport.width);
    background.setHeight(viewport.height - 30);
    background.setColor(0x0f0f0f);
}

void etm::Terminal::userKeyPress(char c) {
    // input.insertChar(c);
    std::cout << "key press: " << c << std::endl;
    dispText(std::string() + c);
    // std::cout << "string'd = " << std::to_string(c) << std::endl;
    flush();
}
void etm::Terminal::userPaste(const std::string &text) {
    // input.insertString(text);
}
void etm::Terminal::userActionKey(actionKey key) {
    // input.action(key);
}
void etm::Terminal::userScroll(float yOffset) {
    output.mouseScroll(yOffset);
}
void etm::Terminal::userClick(bool isPressed, float mouseX, float mouseY) {
    output.mouseClick(isPressed, mouseX, mouseY);
    focused = viewport.hasPoint(mouseX, mouseY);
}
void etm::Terminal::userMove(float mouseX, float mouseY) {
    output.mouseMoved(mouseX, mouseY);
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
    output.render();
    assertGLErr("Terminal.cpp:96");
    // output.render();
    // input.render();
    // resources->bindTextureShader();
    // lines[0].render();
    // testText.render();
    // testImage.render();
}
