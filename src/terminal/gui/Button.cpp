#include "Button.h"

etm::Button::Button(Resources *res): hovering(false) {
}

void etm::Button::setX(float x) {
    area.x = x;
}
void etm::Button::setY(float y) {
    area.y = y;
}
void etm::Button::setWidth(float width) {
    area.width = width;
}
void etm::Button::setHeight(float height) {
    area.height = height;
}

float etm::Button::getX() {
    return area.x;
}
float etm::Button::getY() {
    return area.y;
}
float etm::Button::getWidth() {
    return area.width;
}
float etm::Button::getHeight() {
    return area.height;
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
                onMousePress();
            }
        } else if (onMouseRelease) {
            onMouseRelease();
        }
    }
}
void etm::Button::mouseMove(float mouseX, float mouseY) {
    if (area.hasPoint(mouseX, mouseY)) {
        if (!hovering) {
            hovering = true;
            if (onMouseEnter) {
                onMouseEnter();
            }
        }
    } else if (hovering && onMouseLeave) {
        hovering = false;
        onMouseLeave();
    }
}
