#include "Button.h"

etm::Button::Button(Resources *res): area(res), hovering(false) {
}

void etm::Button::setX(float x) {
    area.setX(x);
}
void etm::Button::setY(float y) {
    area.setY(y);
}
void etm::Button::setWidth(float width) {
    area.setWidth(width);
}
void etm::Button::setHeight(float height) {
    area.setHeight(height);
}
void etm::Button::setColor(const Color &color) {
    area.setColor(color);
}

float etm::Button::getX() {
    return area.getX();
}
float etm::Button::getY() {
    return area.getY();
}
float etm::Button::getWidth() {
    return area.getWidth();
}
float etm::Button::getHeight() {
    return area.getHeight();
}

void etm::Button::setOnMousePress(const callback_t &callback) {
    onMousePress = callback;
}
void etm::Button::setOnMouseRelease(const callback_t &callback) {
    onMouseRelease = callback;
}
void etm::Button::setOnMouseEnter(const callback_t &callback) {
    onMouseEnter = callback;
}
void etm::Button::setOnMouseLeave(const callback_t &callback) {
    onMouseLeave = callback;
}

void etm::Button::mouseClick(bool isPressed, float mouseX, float mouseY) {
    if (area.hasPoint(mouseX, mouseY)) {
        if (isPressed) {
            if (onMousePress) {
                onMousePress(*this);
            }
        } else if (onMouseRelease) {
            onMouseRelease(*this);
        }
    }
}
void etm::Button::mouseMove(float mouseX, float mouseY) {
    if (area.hasPoint(mouseX, mouseY)) {
        if (!hovering) {
            hovering = true;
            if (onMouseEnter) {
                onMouseEnter(*this);
            }
        }
    } else if (hovering && onMouseLeave) {
        hovering = false;
        onMouseLeave(*this);
    }
}

void etm::Button::render() {
    area.render();
}
