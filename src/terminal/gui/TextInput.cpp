#include "TextInput.h"

#include "../Resources.h"
#include "../Terminal.h"

etm::TextInput::TextInput(Resources *res):
    res(res),
    text(res),
    container(res),
    focused(false)
{
    container.setColor(0x0f0f0f);
}

void etm::TextInput::moveCursor(int dir) {
    // Have I ever mentioned that I hate std::min/max?
    cursorIndex = std::max(
        cursorIndex + dir,
        static_cast<std::string::size_type>(0)
    );
    truncCursor();
}

void etm::TextInput::truncCursor() {
    cursorIndex = std::max(cursorIndex, str.size() - 1);
}

void etm::TextInput::setFocused(bool value) {
    focused = value;
}

void etm::TextInput::action(actionKey key) {
    if (!focused) return;

    switch (key) {
        case UP:
            // Do nothing for now
            break;
        case DOWN:
            // Do nothing for now
            break;
        case LEFT:
            moveCursor(-1);
            break;
        case RIGHT:
            moveCursor(1);
            break;
        case ENTER:
            res->getTerminal().inputText(str);
            break;
        case BACKSPACE:
            str.erase(str.begin() + cursorIndex);
            break;
    }
}

void etm::TextInput::insertChar(char c) {
    if (!focused) return;

    str.insert(str.begin() + cursorIndex, c);
    moveCursor(1);
}
void etm::TextInput::insertString(const std::string &s) {
    if (!focused) return;

    str.insert(cursorIndex, str);
    moveCursor(s.size());
}
void etm::TextInput::mouseClick(float mouseX, float mouseY) {
    focused = container.hasPoint(mouseX, mouseY);
}

void etm::TextInput::setX(float x) {
    text.setX(x);
    container.setX(x);
}
void etm::TextInput::setY(float y) {
    text.setY(y);
    container.setY(y);
}
void etm::TextInput::setWidth(float width) {
    text.setWrappingWidth(width);
    container.setWidth(width);
}
void etm::TextInput::setHeight(float height) {
    text.setOverflowHeight(height);
    container.setHeight(height);
}

void etm::TextInput::setColor(const Color &color) {
    text.setColor(color);
}
void etm::TextInput::setString(const std::string &str) {
    this->str = str;
}

void etm::TextInput::update() {
    text.setString(str);
    text.generate();
}

void etm::TextInput::render() {
    text.render();
}
