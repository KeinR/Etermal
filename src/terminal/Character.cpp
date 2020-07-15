#include "Character.h"

#include <iostream>

etm::Character::Character():
    Character('\0') {
}
etm::Character::Character(char value):
    value(value),
    enforceBackColor(false), enforceForeColor(false),
    style(0) {
}

void etm::Character::setValue(char value) {
    this->value = value;
}
void etm::Character::setBackColor(const Color &color) {
    backGColor = color;
    enforceBackColor = true;
}
void etm::Character::setForeColor(const Color &color) {
    foreGColor = color;
    enforceForeColor = true;
}

char etm::Character::getValue() const {
    return value;
}

bool etm::Character::hasBackGC() {
    return enforceBackColor;
}
bool etm::Character::hasForeGC() {
    return enforceForeColor;
}
etm::Color &etm::Character::getBackGC() {
    return backGColor;
}
etm::Color &etm::Character::getForeGC() {
    return foreGColor;
}
