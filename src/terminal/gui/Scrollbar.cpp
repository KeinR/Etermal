#include "Scrollbar.h"

#include <iostream>

#include "../Scroll.h"

etm::Scrollbar::Arrow::Arrow(Scrollbar *parent, float arrowSize, int tickTimeMillis, int arrowWaitMillis, int directionMod):
    parent(parent), button(parent->res), arrow(parent->res),
    directionMod(directionMod), arrowSize(arrowSize),
    pressed(false), active(false),
    tick(tickTimeMillis), wait(arrowWaitMillis)
{
    button.setOnMouseEnter([this](Button &caller)->void{
        caller.setColor(this->bgColor.brighten(-0.1));
    });
    button.setOnMouseLeave([this](Button &caller)->void{
        caller.setColor(this->bgColor);
        this->pressed = false;
    });
    button.setOnMousePress([this](Button &caller)->void{
        caller.setColor(this->bgColor.brighten(-0.3));
        this->pressed = true;
        wait.start();
    });
    button.setOnMouseRelease([this](Button &caller)->void{
        if (this->pressed) {
            caller.setColor(this->bgColor.brighten(-0.1));
            this->pressed = false;
        }
    });
    if (directionMod == 1) {
        arrow.setRotation(180);
    }
}

void etm::Scrollbar::Arrow::calcArrowW() {
    arrow.setX(button.getX() + button.getWidth() / 2 * (1.0f - arrowSize));
    arrow.setWidth(button.getWidth() * arrowSize);
}

void etm::Scrollbar::Arrow::calcArrowH() {
    arrow.setY(button.getY() + button.getHeight() / 2 * (1.0f - arrowSize));
    arrow.setHeight(button.getHeight() * arrowSize);
}

void etm::Scrollbar::Arrow::setX(float x) {
    button.setX(x);
    calcArrowW();
}
void etm::Scrollbar::Arrow::setY(float y) {
    button.setY(y);
    calcArrowH();
}
void etm::Scrollbar::Arrow::setWidth(float width) {
    button.setWidth(width);
    calcArrowW();
}
void etm::Scrollbar::Arrow::setHeight(float height) {
    button.setHeight(height);
    calcArrowH();
}

float etm::Scrollbar::Arrow::getX() {
    return button.getX();
}
float etm::Scrollbar::Arrow::getY() {
    return button.getY();
}
float etm::Scrollbar::Arrow::getWidth() {
    return button.getWidth();
}
float etm::Scrollbar::Arrow::getHeight() {
    return button.getHeight();
}

void etm::Scrollbar::Arrow::setColor(const Color &color) {
    bgColor = color;
    button.setColor(bgColor);
}
void etm::Scrollbar::Arrow::setArrowColor(const Color &color) {
    arrowColor = color;
    if (active) {
        arrow.setColor(arrowColor);
    } else {
        arrow.setColor(arrowColor.brighten(5));
    }
}
void etm::Scrollbar::Arrow::mouseClick(bool isPressed, float mouseX, float mouseY) {
    if (active) {
        button.mouseClick(isPressed, mouseX, mouseY);
    }
}
void etm::Scrollbar::Arrow::mouseMove(float mouseX, float mouseY) {
    if (active) {
        button.mouseMove(mouseX, mouseY);
    }
}

void etm::Scrollbar::Arrow::setActive(bool value) {
    if (active != value) {
        active = value;
        if (active) {
            arrow.setColor(arrowColor);
        } else {
            button.setColor(bgColor);
            arrow.setColor(arrowColor.brighten(5));
            pressed = false;
        }
    }
}

void etm::Scrollbar::Arrow::render() {
    if (pressed && wait.hasEnded() && tick.hasEnded()) {
        parent->scroll->scrollByAlign(directionMod);
        parent->update();
        tick.start();
    }
    button.render();
    arrow.render();
}

etm::Scrollbar::Scrollbar(Resources *res, Scroll &scroll):
    res(res),
    scroll(&scroll),
    bar(res),
    slider(res),
    dragging(false),
    dragY(0.0d),
    showingSlider(false),
    sideMargin(1),
    upArrow(this, 0.50f, 300, 1000, -1),
    downArrow(this, 0.50f, 300, 1000, 1)
{
    upArrow.setHeight(20);
    downArrow.setHeight(20);
    upArrow.setArrowColor(0x1f1f1f);
    downArrow.setArrowColor(0x1f1f1f);
}

void etm::Scrollbar::calcSliderX() {
    slider.setX(bar.getX() + sideMargin);
}

void etm::Scrollbar::calcSliderY() {
    slider.setY(bar.getY() + upArrow.getHeight() + (bar.getHeight() - slider.getHeight() - upArrow.getHeight() - downArrow.getHeight()) * scroll->getOffset() / scroll->getMaxOffset());
}

void etm::Scrollbar::calcSliderWidth() {
    slider.setWidth(bar.getWidth() - sideMargin * 2);
}

void etm::Scrollbar::calcArrows() {
    upArrow.setX(bar.getX() + sideMargin);
    downArrow.setX(bar.getX() + sideMargin);
    upArrow.setY(bar.getY());
    downArrow.setY(bar.getY() + bar.getHeight() - downArrow.getHeight());
    upArrow.setWidth(bar.getWidth() - sideMargin * 2);
    downArrow.setWidth(bar.getWidth() - sideMargin * 2);
    calcSliderY();
}

void etm::Scrollbar::setX(int x) {
    bar.setX(x);
    calcSliderX();
    calcArrows();
}
void etm::Scrollbar::setY(int y) {
    bar.setY(y);
    calcArrows();
}
void etm::Scrollbar::setWidth(int width) {
    bar.setWidth(width);
    calcArrows();
    calcSliderWidth();
}
void etm::Scrollbar::setHeight(int height) {
    bar.setHeight(height);
    calcArrows();
}
void etm::Scrollbar::setSideMargin(int margin) {
    sideMargin = margin;
    calcSliderX();
    calcSliderWidth();
    calcArrows();
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
    upArrow.setColor(color);
    downArrow.setColor(color);
}
void etm::Scrollbar::setSliderColor(const Color &color) {
    slider.setColor(color);
    sliderColor = color;
    sliderColorHover = color.brighten(-0.2);
}

void etm::Scrollbar::update() {
    showingSlider = scroll->getGrossHeight() > bar.getHeight();
    if (showingSlider) {
        const float maxHeight = bar.getHeight() - upArrow.getHeight() - downArrow.getHeight();
        slider.setHeight(maxHeight * maxHeight / scroll->getGrossHeight());
        calcSliderY();
    }
    upArrow.setActive(showingSlider);
    downArrow.setActive(showingSlider);
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
    upArrow.mouseClick(isPressed, mouseX, mouseY);
    downArrow.mouseClick(isPressed, mouseX, mouseY);
}
void etm::Scrollbar::mouseMove(float mouseX, float mouseY) {
    if (dragging) {
        const float extraHeight = bar.getHeight() - slider.getHeight() - upArrow.getHeight() - downArrow.getHeight();
        scroll->scroll((mouseY - dragY)  * scroll->getMaxOffset() / extraHeight);
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
    upArrow.mouseMove(mouseX, mouseY);
    downArrow.mouseMove(mouseX, mouseY);
}
void etm::Scrollbar::render() {
    bar.render();
    if (showingSlider) {
        slider.render();
    }
    upArrow.render();
    downArrow.render();
}
