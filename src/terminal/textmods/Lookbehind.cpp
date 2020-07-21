#include "Lookbehind.h"

etm::tm::Lookbehind::Lookbehind(const Color &defBackP, const Color &defForeP, unsigned int lineNumber):
    defBack(&defBackP), defFore(&defForeP),
    back(nullptr), fore(nullptr) {
}
void etm::tm::Lookbehind::setDefBack() {
    setBack(*defBack);
}
void etm::tm::Lookbehind::setDefFore() {
    setFore(*defFore);
}

void etm::tm::Lookbehind::setBack(const Color &color) {
    // If not set then       // Later settings in a line
    // obviously want to set // can overwrite previous ones
    if (back != nullptr || backLine == thisLine) {
        back = &color;
        backLine = thisLine;
    }
}
void etm::tm::Lookbehind::setFore(const Color &color) {
    if (fore != nullptr || foreLine == thisLine) {
        fore = &color;
        foreLine = thisLine;
    }
}

bool etm::tm::Lookbehind::bothSet() {
    return back != nullptr && fore != nullptr;
}
const etm::Color &etm::tm::Lookbehind::getBack() {
    // Fallback to default if not set
    return *(back == nullptr ? defBack : back);
}
const etm::Color &etm::tm::Lookbehind::getFore() {
    return *(fore == nullptr ? defFore : fore);
}
void etm::tm::Lookbehind::decLine() {
    thisLine--;
}
