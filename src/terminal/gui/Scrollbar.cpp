#include "Scrollbar.h"

#include <iostream>

#include "../Scroll.h"

etm::Scrollbar::Scrollbar(Resources *res, Scroll &scroll):
    res(res),
    scroll(&scroll),
    bar(res),
    slider(res),
    dragging(false),
    dragY(0.0d),
    showingSlider(false),
    sideMargin(1)
{
}

void etm::Scrollbar::calcSliderX() {
    slider.setX(bar.getX() + sideMargin);
}

void etm::Scrollbar::calcSliderY() {
    slider.setY((bar.getHeight() - slider.getHeight()) * scroll->getOffset() / scroll->getMaxOffset());
}

void etm::Scrollbar::calcSliderWidth() {
    slider.setWidth(bar.getWidth() - sideMargin * 2);
}

void etm::Scrollbar::setX(int x) {
    bar.setX(x);
    calcSliderX();
}
void etm::Scrollbar::setY(int y) {
    bar.setY(y);
}
void etm::Scrollbar::setWidth(int width) {
    bar.setWidth(width);
    calcSliderWidth();
}
void etm::Scrollbar::setHeight(int height) {
    bar.setHeight(height);
}
void etm::Scrollbar::setSideMargin(int margin) {
    sideMargin = margin;
    calcSliderX();
    calcSliderWidth();
}

int etm::Scrollbar::getX() {
    return bar.getX();
}
int etm::Scrollbar::getY() {
    return bar.getY();
}
int etm::Scrollbar::getWidth() {
    return bar.getWidth();
}
int etm::Scrollbar::getHeight() {
    return bar.getHeight();
}
int etm::Scrollbar::getSideMargin() {
    return sideMargin;
}

void etm::Scrollbar::setBarColor(const Color &color) {
    bar.setColor(color);
}
void etm::Scrollbar::setSliderColor(const Color &color) {
    slider.setColor(color);
    sliderColor = color;
    sliderColorHover = color.brighten(-0.2);
}

void etm::Scrollbar::update() {
    showingSlider = scroll->getGrossHeight() > bar.getHeight();
    if (showingSlider) {
        slider.setHeight(bar.getHeight() * bar.getHeight() / scroll->getGrossHeight());
        calcSliderY();
    }
}
void etm::Scrollbar::mouseClick(bool isPressed, float mouseX, float mouseY) {
    if (isPressed) {
        if (bar.hasPoint(mouseX, mouseY)) {
            dragging = true;
            dragY = mouseY;
        }
    } else {
        dragging = false;
    }
}
void etm::Scrollbar::mouseMove(float mouseX, float mouseY) {
    if (dragging) {
        scroll->scroll((mouseY - dragY)  * scroll->getMaxOffset() / (bar.getHeight() - slider.getHeight()));
        dragY = mouseY;
        calcSliderY();
    } else if (sliderHovering) {
        if (!slider.hasPoint(mouseX, mouseY)) {
            slider.setColor(sliderColor);
            sliderHovering = false;
        }
    } else if (slider.hasPoint(mouseX, mouseY)) {
        slider.setColor(sliderColorHover);
        sliderHovering = true;
    }
}
void etm::Scrollbar::render() {
    bar.render();
    if (showingSlider) {
        slider.render();
    }
}
