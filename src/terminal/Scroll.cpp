#include "Scroll.h"

#include <iostream>
#include <cmath>

#include "Resources.h"

etm::Scroll::Scroll():
    grossHeight(1), // Prevent /0 error in scrollbar
    netHeight(1), // Prevent /0 error in scrollbar
    offset(0),
    maxOffset(0)
{
}

void etm::Scroll::recalc() {
    maxOffset = std::max(grossHeight - netHeight, 0);
    offset = std::min(offset, maxOffset);
}

void etm::Scroll::setNetHeight(int height) {
    netHeight = height;
    recalc();
}

void etm::Scroll::setGrossHeight(int height) {
    grossHeight = height;
    recalc();
}

int etm::Scroll::getNetHeight() {
    return netHeight;
}
int etm::Scroll::getGrossHeight() {
    return grossHeight;
}
float etm::Scroll::getOffset() {
    return offset;
}
float etm::Scroll::getMaxOffset() {
    return maxOffset;
}

void etm::Scroll::scroll(float value) {
    offset = std::max(
        std::min(
            offset + value,
            maxOffset
        ),
        0.0f
    );
}

void etm::Scroll::jump() {
    offset = maxOffset;
}
