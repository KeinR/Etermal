#include "Terminal.h"

#include "render/glfw.h"
#include "util/util.h" // TEMP

etm::Terminal::Terminal():
    resources(new Resources(*this)),
    viewport(0, 0, 400, 400),
    testText(resources.get()),
    testImage(resources.get()),
    output(resources.get()),
    input(resources.get()),
    outputBG(resources.get()),
    inputBG(resources.get()),
    shell(nullptr)
{
    updatePosition();

    testImage.loadImage("C:\\Users\\orion\\Pictures\\1477927 (1).jpg");
    // testImage.loadImage("renderLines.png");
    testImage.setX(0);
    testImage.setY(300);
    testImage.setWidth(75);
    testImage.setHeight(93);

    // TEMP
    lines.emplace_back(resources.get());
    output.getElements().push_back(&lines[0]);
    dispText("fell like a rain");
    flush();
    testText.setX(40);
    testText.setY(100);
    testText.setString("DOes this work?");
    testText.setWrappingWidth(100);
    testText.setFontSize(30);
    testText.generate();
}

// The shell is where user input will be directed
void etm::Terminal::setShell(Shell &shell) {
    this->shell = &shell;
}

void etm::Terminal::dispText(const std::string &str) {
    line1text += str;
}

void etm::Terminal::flush() {
    lines[0].setString(line1text);
    lines[0].generate();
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
    outputBG.setColor(0x29b340);

    input.setX(viewport.x);
    input.setY(viewport.y + viewport.height - 30);
    input.setWidth(viewport.width);
    input.setHeight(30);
    inputBG.setX(viewport.x);
    inputBG.setY(viewport.y + viewport.height - 30);
    inputBG.setWidth(viewport.width);
    inputBG.setHeight(30);
    inputBG.setColor(0x3279a8);
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
    glEnable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    assertGLErr("Terminal.cpp:91");
    resources->bindPrimitiveShader();
    assertGLErr("Terminal.cpp:92");
    outputBG.render();
    assertGLErr("Terminal.cpp:94");
    inputBG.render();
    assertGLErr("Terminal.cpp:96");
    // output.render();
    // input.render();
    resources->bindTextureShader();
    lines[0].render();
    testText.render();
    testImage.render();
}
