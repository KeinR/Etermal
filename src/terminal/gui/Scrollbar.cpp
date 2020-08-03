#include "Scrollbar.h"

#include "../Scroll.h"
#include "../Resources.h"

static constexpr int disabledArrowBrightness = 5;

etm::Scrollbar::Arrow::Arrow(Scrollbar *parentP, int tickTimeMillis, int arrowWaitMillis, int directionMod):
    parent(parentP),
    arrow(parent->res),
    directionMod(directionMod),
    pressed(false), active(false),
    tick(tickTimeMillis), wait(arrowWaitMillis)
{
    if (directionMod == 1) {
        // If facing down, flip
        arrow.setRotation(180);
    }
}

void etm::Scrollbar::Arrow::doScroll() {
    parent->scroll->scrollByAlign(directionMod);
}

void etm::Scrollbar::Arrow::setX(float x) {
    // Must sync the arrow and button areas
    arrow.setX(x);
}
void etm::Scrollbar::Arrow::setY(float y) {
    arrow.setY(y);
}
void etm::Scrollbar::Arrow::setWidth(float width) {
    arrow.setWidth(width);
}
void etm::Scrollbar::Arrow::setHeight(float height) {
    arrow.setHeight(height);
}

float etm::Scrollbar::Arrow::getX() {
    return arrow.getX();
}
float etm::Scrollbar::Arrow::getY() {
    return arrow.getY();
}
float etm::Scrollbar::Arrow::getWidth() {
    return arrow.getWidth();
}
float etm::Scrollbar::Arrow::getHeight() {
    return arrow.getHeight();
}

void etm::Scrollbar::Arrow::setScrollCooldown(int millis) {
    tick.setTime(millis);
}

void etm::Scrollbar::Arrow::setScrollWait(int millis) {
    wait.setTime(millis);
}

void etm::Scrollbar::Arrow::setColor(const Color &color) {
    bgColor = color;
    arrow.setBackColor(bgColor);
}
void etm::Scrollbar::Arrow::setArrowColor(const Color &color) {
    arrowColor = color;
    // If we didn't check this, given active = false
    // later calls to set the arrow color would mess up
    // the colors until `active` is toggled again, because
    // input events are ignored if `active` is false
    if (active) {
        arrow.setForeColor(arrowColor);
    } else {
        arrow.setForeColor(arrowColor.brighten(disabledArrowBrightness));
    }
}
void etm::Scrollbar::Arrow::mouseClick(bool isPressed, float mouseX, float mouseY) {
    // If not active, not point in processing events
    if (active) {
        if (arrow.hasPoint(mouseX, mouseY)) {
            if (isPressed) {
                arrow.setBackColor(bgColor.brighten(-0.3));
                pressed = true;
                doScroll();
                wait.start();
            } else if (pressed) {
                // If we didn't check if it was pressed,
                // then the user could press, move the mouse out
                // of the button area, and release.
                // This func would be called _after_ the leave
                // func, causing the arrow to have a grayed out
                // tint.
                arrow.setBackColor(bgColor.brighten(-0.1));
                pressed = false;
                parent->getRes()->invalidateDisplay();
            }
        }
    }
}
void etm::Scrollbar::Arrow::mouseMove(float mouseX, float mouseY) {
    if (active) {
        if (arrow.hasPoint(mouseX, mouseY)) {
            if (!hovering) {
                hovering = true;
                arrow.setBackColor(bgColor.brighten(-0.1));
                parent->getRes()->invalidateDisplay();
            }
        } else if (hovering) {
            hovering = false;
            arrow.setBackColor(bgColor);
            pressed = false;
            parent->getRes()->invalidateDisplay();
        }
    }
}

void etm::Scrollbar::Arrow::setActive(bool value) {
    if (active != value) {
        active = value;
        if (active) {
            arrow.setForeColor(arrowColor);
        } else {
            arrow.setBackColor(bgColor);
            arrow.setForeColor(arrowColor.brighten(disabledArrowBrightness));
            pressed = false;
        }
    }
}

void etm::Scrollbar::Arrow::render() {
    arrow.render();
}

void etm::Scrollbar::Arrow::animate() {
    // `pressed` asserts that the user is actually
    // pressing down on the arrow, `wait`, checks
    // that the press has been going on long enough
    // to enable "scroll repeat" or whatever you call it,
    // and `tick` keeps the scroll interval to a minimum.
    if (pressed && wait.hasEnded() && tick.hasEnded()) {
        doScroll();
        tick.start();
    }
}

etm::Scrollbar::Scrollbar(Resources *res, Scroll &scroll):
    res(res),
    scroll(&scroll),
    bar(res), slider(res),
    dragging(false),
    dragY(0),
    showingSlider(false),
    sideMargin(1),
    upArrow(this, 100, 300, -1),
    downArrow(this, 100, 300, 1)
{
    upArrow.setHeight(20);
    downArrow.setHeight(20);
    upArrow.setArrowColor(0x1f1f1f);
    downArrow.setArrowColor(0x1f1f1f);
}

void etm::Scrollbar::setScroll(Scroll &scroll) {
    this->scroll = &scroll;
}

void etm::Scrollbar::setScrollCooldown(int millis) {

}

void etm::Scrollbar::setScrollWait(int millis) {

}

void etm::Scrollbar::calcSliderX() {
    slider.setX(bar.getX() + sideMargin);
}

void etm::Scrollbar::calcSliderY() {
    // bar.getY() + upArrow.getHeight() // Topmost-y coordinate possible for slider
    // + (bar.getHeight() - slider.getHeight() - upArrow.getHeight() - downArrow.getHeight()) // Max y offset from that
    // * scroll->getOffset() / scroll->getMaxOffset() // Percent scrolled
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

etm::Resources *etm::Scrollbar::getRes() {
    return res;
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
    // Only show the slider if there's actually available scroll
    showingSlider = scroll->getGrossHeight() > scroll->getNetHeight();
    if (showingSlider) {
        // The maximum height possible for the slider
        const float maxHeight = bar.getHeight() - upArrow.getHeight() - downArrow.getHeight();
        // Sync slider height with the percent of text viewable via the viewport
        slider.setHeight(maxHeight * scroll->getNetHeight() / scroll->getGrossHeight());
        calcSliderY();
    }
    upArrow.setActive(showingSlider);
    downArrow.setActive(showingSlider);
}

void etm::Scrollbar::updateScroll() {
    calcSliderY();
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
        // Max y offset for the slider
        const float extraHeight = bar.getHeight() - slider.getHeight() - upArrow.getHeight() - downArrow.getHeight();
        // Map mouse pixel distance to the proportion of the max scroll and slider offsets 
        scroll->scroll((mouseY - dragY)  * scroll->getMaxOffset() / extraHeight);
        dragY = mouseY;
    } else if (sliderHovering) {
        if (!slider.hasPoint(mouseX, mouseY)) {
            slider.setColor(sliderColor);
            sliderHovering = false;
            res->invalidateDisplay();
        }
    } else if (slider.hasPoint(mouseX, mouseY)) {
        slider.setColor(sliderColorHover);
        sliderHovering = true;
        res->invalidateDisplay();
    }
    upArrow.mouseMove(mouseX, mouseY);
    downArrow.mouseMove(mouseX, mouseY);
}
void etm::Scrollbar::render() {
    bar.render();
    if (showingSlider) {
        slider.render();
    }

    // Note that this means that the shader
    // doesn't need to be set later on when
    // rendering the text
    res->bindTextShader();

    upArrow.render();
    downArrow.render();
}

void etm::Scrollbar::animate() {
    upArrow.animate();
    downArrow.animate();
}
