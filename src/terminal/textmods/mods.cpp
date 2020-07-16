#include "mods.h"

#include <iostream>

#include "TextState.h"

etm::tm::Background::Background(const Color &color): color(color) {
}
void etm::tm::Background::run(TextState &state) {
    state.setBack(color);
}

etm::tm::Foreground::Foreground(const Color &color): color(color) {
}
void etm::tm::Foreground::run(TextState &state) {
    state.setFore(color);
}

etm::tm::RevBackground::RevBackground() {
}
void etm::tm::RevBackground::run(TextState &state) {
    state.setDefBack();
}

etm::tm::RevForeground::RevForeground() {
}
void etm::tm::RevForeground::run(TextState &state) {
    state.setDefFore();
}

etm::tm::RevBackFore::RevBackFore() {
}
void etm::tm::RevBackFore::run(TextState &state) {
    state.setDefBack();
    state.setDefFore();
}
