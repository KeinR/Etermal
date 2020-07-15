#include "Character.h"

etm::Character::Character():
    value(0), style(0) {
}
etm::Character::Character(char value):
    value(value), style(0) {
}

void etm::Character::setValue(char value) {
    this->value = value;
}
void etm::Character::setColor(const Color &color) {
    this->color = color;
}

char etm::Character::getValue() const {
    return value;
}
etm::Color &etm::Character::getColor() {
    return color;
}

