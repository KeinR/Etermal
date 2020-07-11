#include "Terminal.h"

#include "render/glfw.h"

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 100, 100),
    output(resources.get()),
    input(resources.get()),
    outputBG(resources.get()),
    inputBG(resources.get()),
    shell(nullptr)
{
    updatePosition();

    // TEMP
    lines.emplace_back(resources.get());
    output.getElements().push_back(&lines[0]);
}

// The shell is where user input will be directed
void etm::Terminal::setShell(Shell &shell) {
    this->shell = &shell;
}

void etm::Terminal::dispText(const std::string &str) {
    line1text += str;
    lines[0].setString(line1text);
}

void etm::Terminal::inputText(const std::string &str) {
    // Send info to shell
}

void etm::Terminal::setX(float x) {
    viewport.x = x;
}
void etm::Terminal::setY(float y) {
    viewport.y = y;
}
void etm::Terminal::setWidth(float width) {
    viewport.width = width;
}
void etm::Terminal::setHeight(float height) {
    viewport.height = height;
}

void etm::Terminal::updatePosition() {
    output.setX(viewport.x);
    output.setY(viewport.y);
    output.setWidth(viewport.width);
    output.setHeight(viewport.height - 30);
    outputBG.setX(viewport.x);
    outputBG.setY(viewport.y);
    outputBG.setWidth(viewport.width);
    outputBG.setHeight(viewport.height - 30);

    input.setX(viewport.x);
    input.setY(viewport.y + viewport.height - 30);
    input.setWidth(viewport.width);
    input.setHeight(30);
    outputBG.setX(viewport.x);
    outputBG.setY(viewport.y + viewport.height - 30);
    outputBG.setWidth(viewport.width);
    outputBG.setHeight(30);
}

void etm::Terminal::userKeyPress(char c) {
    input.insertChar(c);
}
void etm::Terminal::userPaste(const std::string &text) {
    input.insertString(text);
}
void etm::Terminal::userActionKey(actionKey key) {
    input.action(key);
}
void etm::Terminal::userScroll(float yOffset) {
    output.mouseScroll(yOffset);
}
void etm::Terminal::userClick(bool isPressed, float mouseX, float mouseY) {
    output.mouseClick(isPressed, mouseX, mouseY);
    input.mouseClick(mouseX, mouseY);
}
void etm::Terminal::userMove(float mouseX, float mouseY) {
    output.mouseMoved(mouseX, mouseY);
}

void etm::Terminal::render() {
    glEnable(GL_SCISSOR_TEST);
    outputBG.render();
    inputBG.render();
    output.render();
    input.render();
}
