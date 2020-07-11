#include "ScrollBox.h"

#include <iostream>
#include <cmath>

#include "../render/glfw.h"
#include "../Resources.h"
#include "Text.h"
#include "../util/util.h"

etm::ScrollBox::Scrollbar::Scrollbar(ScrollBox &wrapper):
    wrapper(&wrapper),
    bar(wrapper.res),
    dragging(false),
    dragY(0.0d),
    showing(false) {
}

void etm::ScrollBox::Scrollbar::setWidth(int width) {
    bar.setWidth(width);
}
void etm::ScrollBox::Scrollbar::setColor(const Color &color) {
    bar.setColor(color);
}
void etm::ScrollBox::Scrollbar::init(int scrollOffset, int collectiveHeight) {
    showing = collectiveHeight > wrapper->model.height;
    if (showing) {
        bar.setHeight(wrapper->model.height / collectiveHeight * wrapper->model.height);
        bar.setY(wrapper->model.y + scrollOffset / (collectiveHeight - wrapper->model.height) * (wrapper->model.height - bar.getHeight()));
        bar.setX(wrapper->model.x + wrapper->model.width - bar.getWidth());
    }
}
void etm::ScrollBox::Scrollbar::mouseClick(bool isPressed, float mouseX, float mouseY) {
    if (isPressed) {
        dragging = true;
        dragY = mouseY;
    } else {
        dragging = false;
    }
}
void etm::ScrollBox::Scrollbar::mouseMoved(float mouseX, float mouseY) {
    if (dragging) {
        wrapper->scroll(mouseY - dragY);
        dragY = mouseY;
    }
}
void etm::ScrollBox::Scrollbar::render() {
    if (showing) {
        wrapper->res->bindPrimitiveShader();
        bar.render();
    }
}

etm::ScrollBox::ScrollBox(Resources *res):
    res(res),
    scrollOffset(0),
    elementSpacing(3),
    sideSpacing(5),
    hovering(false),
    lastCollectiveHeight(0),
    maxScrollOffest(0),
    scrollbar(*this)
{
    scrollbar.setWidth(10);
    scrollbar.setColor(0x626569);
}

void etm::ScrollBox::setX(int x) {
    model.x = x;
}
void etm::ScrollBox::setY(int y) {
    model.y = y;
}
void etm::ScrollBox::setHeight(int height) {
    model.height = height;
}
void etm::ScrollBox::setWidth(int width) {
    model.width = width;
}
int etm::ScrollBox::getX() {
    return model.x;
}
int etm::ScrollBox::getY() {
    return model.y;
}
int etm::ScrollBox::getHeight() {
    return model.height;
}
int etm::ScrollBox::getWidth() {
    return model.width;
}

void etm::ScrollBox::setScrollbarWidth(int width) {
    scrollbar.setWidth(width);
}
void etm::ScrollBox::setScrollbarColor(const Color &color) {
    scrollbar.setColor(color);
}

etm::ScrollBox::elements_t &etm::ScrollBox::getElements() {
    return elements;
}

void etm::ScrollBox::update() {

    const int width = model.width - sideSpacing * 2;
    const int x = model.x + (model.width - width) / 2;

    lastCollectiveHeight = 0;
    for (Text *&e : elements) {
        e->setWrappingWidth(width);
        e->setX(x);
        e->generate();
        lastCollectiveHeight += e->getHeight() + elementSpacing;
    }
    // lastCollectiveHeight -= elementSpacing;
    maxScrollOffest = std::max(lastCollectiveHeight - std::round(model.height) + elementSpacing, 0.0f);

    updateScroll();
}

void etm::ScrollBox::updateScroll() {

    const int scrollOffsetInt = std::round(scrollOffset);

    int y = model.y + elementSpacing - scrollOffsetInt;
    for (Text *&e : elements) {
        e->setY(y);
        y += e->getHeight() + elementSpacing;
    }

    scrollbar.init(scrollOffsetInt, lastCollectiveHeight);
}

void etm::ScrollBox::render() {
    res->bindTextureShader();

    setScissor(model.x, model.y, model.width, model.height);

    for (Text *&e : elements) {
        e->render();
    }

    resetScissor();

    if (hovering) {
        scrollbar.render();
    }

}

void etm::ScrollBox::mouseMoved(float mouseX, float mouseY) {
    hovering = model.hasPoint(mouseX, mouseY);
    scrollbar.mouseMoved(mouseX, mouseY);
}

void etm::ScrollBox::mouseClick(bool isPressed, float mouseX, float mouseY) {
    scrollbar.mouseClick(isPressed, mouseX, mouseY);
}

void etm::ScrollBox::scroll(float value) {
    scrollOffset = std::max(
        std::min(
            scrollOffset + value,
            maxScrollOffest
        ),
        0.0d
    );
    updateScroll();
}

void etm::ScrollBox::mouseScroll(float yOffset) {
    // double mouseX, mouseY;
    // Window::getCursorPos(mouseX, mouseY);
    // if (model.hasPointInc(mouseX, mouseY)) {
    //     scroll(-yOffset * 4); // TODO: Scroll speed factor (4)
    // }
    if (hovering) {
        scroll(-yOffset * 4); // TODO: Scroll speed factor (4)
    }
}
