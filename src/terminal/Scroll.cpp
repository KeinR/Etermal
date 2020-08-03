#include "Scroll.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include "Resources.h"

etm::Scroll::Scroll(Resources *res):
    res(res),
    grossHeight(1), // Prevent /0 error in scrollbar
    netHeight(1), // Prevent /0 error in scrollbar
    offset(0),
    maxOffset(0),
    align(1),
    alignBuffer(0)
{
}

void etm::Scroll::setAlign(float value) {
    align = value;
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
    alignBuffer += value;
    if (alignBuffer >= align) {
        // Flush
        offset = std::min(
                            // Account for overflow
            offset + align * std::floor(alignBuffer / align),
            maxOffset
        );
        // Mod to get remaining scroll
        alignBuffer = std::fmod(alignBuffer, align);
        res->notifyScroll();
    } else if (alignBuffer <= -align) {
        // Flush
        // NOTE: We use ceil because APPARENTLY
        // floor and ceil aren't aligned to signs, ie,
        // floor goes most negative, ceil most positive.
        offset = std::max(
            offset + align * std::ceil(alignBuffer / align),
            0.0f
        );
        alignBuffer = std::fmod(alignBuffer, -align);
        res->notifyScroll();
    }
}

void etm::Scroll::scrollByAlign(int ammount) {
    scroll(ammount * align);
}

void etm::Scroll::jump() {
    offset = maxOffset;
}
