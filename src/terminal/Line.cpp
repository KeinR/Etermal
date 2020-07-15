#include "Line.h"

#include <iostream>
#include <cmath>

etm::Line::Line():
    newline(false),
    startSpace(false) {
}
etm::Character &etm::Line::operator[](size_type index) {
    return string[index];
}
etm::Line::size_type etm::Line::size() {
    return string.size();
}
void etm::Line::append(const Character &c) {
    string.push_back(c);
}
void etm::Line::insert(size_type index, const Character &c) {
    string.insert(string.begin() + index, c);
}
void etm::Line::insertStr(size_type index, const string_t &str) {
    string.insert(string.begin() + index, str.begin(), str.end());
}
void etm::Line::erase(size_type index, size_type length) {
    string.erase(string.begin() + index, string.begin() + std::min(index + length, string.size()));
}
void etm::Line::appendStr(const string_t &str) {
    insertStr(string.size(), str);
}
void etm::Line::appendOther(const Line &other) {
    newline |= other.newline;
    if (other.startSpace) {
        append(Character(' '));
    }
    appendStr(other.string);
}
void etm::Line::popBack() {
    string.pop_back();
}

etm::Line::string_t etm::Line::substr(size_type startInc, size_type length) {
    string_t result;
    result.insert(result.end(), string.begin() + startInc, string.begin() + std::min(startInc + length, string.size()));
    return result;
}

void etm::Line::copyTo(string_t &str) {
    str.insert(str.end(), string.begin(), string.end());
}

void etm::Line::reserve(size_type size) {
    string.reserve(size);
}

void etm::Line::setNewline(bool value) {
    newline = value;
}
void etm::Line::setStartSpace(bool value) {
    startSpace = value;
}

bool etm::Line::hasNewline() {
    return newline;
}
bool etm::Line::hasStartSpace() {
    return startSpace;
}
