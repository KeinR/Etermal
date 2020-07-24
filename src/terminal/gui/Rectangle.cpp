#include "Rectangle.h"

#include "../Resources.h"

etm::Rectangle::Rectangle(Resources *res): res(res) {
}

void etm::Rectangle::setColor(const Color &color) {
    this->color = color;
}

void etm::Rectangle::setX(float x) {
    model.x = x;
}
void etm::Rectangle::setY(float y) {
    model.y = y;
}
void etm::Rectangle::setWidth(float width) {
    model.width = width;
}
void etm::Rectangle::setHeight(float height) {
    model.height = height;
}

float etm::Rectangle::getX() {
    return model.x;
}
float etm::Rectangle::getY() {
    return model.y;
}
float etm::Rectangle::getWidth() {
    return model.width;
}
float etm::Rectangle::getHeight() {
    return model.height;
}

bool etm::Rectangle::hasPoint(float x, float y) {
    return model.hasPoint(x, y);
}

void etm::Rectangle::render() {
    model.set(res);
    color.set(res->getShader());
    res->renderRectangle();
}
