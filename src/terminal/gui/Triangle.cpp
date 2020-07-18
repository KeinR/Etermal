#include "Triangle.h"

#include "../Resources.h"

etm::Triangle::Triangle(Resources *res): res(res) {
}

void etm::Triangle::setX(float x) {
    model.x = x;
}
void etm::Triangle::setY(float y) {
    model.y = y;
}
void etm::Triangle::setWidth(float width) {
    model.width = width;
}
void etm::Triangle::setHeight(float height) {
    model.height = height;
}
void etm::Triangle::setRotation(float degrees) {
    model.rotation = degrees;
}
void etm::Triangle::setBackColor(const Color &color) {
    backgroundColor = color;
}
void etm::Triangle::setForeColor(const Color &color) {
    foregroundColor = color;
}

float etm::Triangle::getX() {
    return model.x;
}
float etm::Triangle::getY() {
    return model.y;
}
float etm::Triangle::getWidth() {
    return model.width;
}
float etm::Triangle::getHeight() {
    return model.height;
}
float etm::Triangle::getRotation() {
    return model.rotation;
}

void etm::Triangle::render() {
    model.set(res->getShader());
    backgroundColor.setBackground(res->getShader());
    foregroundColor.setForeground(res->getShader());
    res->renderTriangle();
}
