#include "Line.h"

#include <algorithm>

#include "codec.h"


etm::Line::iterator::iterator(Line *parent, size_type index):
    parent(parent), index(index)
{
}

bool etm::Line::iterator::valid() {
    // Don't check greater than zero too because it's unsigned
    return index < parent->dejureSize();
}

bool etm::Line::iterator::operator<(const iterator &other) {
    return index < other.index;
}
bool etm::Line::iterator::operator<=(const iterator &other) {
    return index <= other.index;
}
bool etm::Line::iterator::operator>(const iterator &other) {
    return index > other.index;
}
bool etm::Line::iterator::operator>=(const iterator &other) {
    return index >= other.index;
}
bool etm::Line::iterator::operator==(const iterator &other) {
    return index == other.index;
}

etm::Line::iterator etm::Line::iterator::operator-(size_type distance) {
    iterator result = *this;
    result -= distance;
    return result;
}
etm::Line::iterator etm::Line::iterator::operator+(size_type distance) {
    iterator result = *this;
    result += distance;
    return result;
}
void etm::Line::iterator::operator-=(size_type distance) {
    for (size_type i = 0; i < distance && valid(); i++) {
        operator--();
    }
}
void etm::Line::iterator::operator+=(size_type distance) {
    for (size_type i = 0; i < distance && valid(); i++) {
        operator++();
    }
}
void etm::Line::iterator::operator--() {
    index--;
    // Might underflow because unsigned
    if (index >= parent->dejureSize()) return;
    // Don't need to test if index is invalid after this - 
    // utf8::lookbehind will return 0 if the new index would
    // be invalid.
    index -= utf8::lookbehind(parent->getString(), index);
    while (ctrl::testEnd(parent->getDejure(index))) {
        index -= ctrl::getJump() + 1;
    }
    index -= utf8::lookbehind(parent->getString(), index);
}
void etm::Line::iterator::operator++() {
    if (index >= parent->dejureSize()) return;
    index += utf8::test(parent->getDejure(index));
    // Don't trust the offset given by the byte header.
    if (index >= parent->dejureSize()) return;
    while (ctrl::testStart(parent->getDejure(index))) {
        index += ctrl::getJump() + 1;
    }
}

etm::Line::codepoint etm::Line::iterator::operator*() {
    const int size = utf8::test(parent->getDejure(index));
    return codepoint(parent->getString().begin() + index, parent->getString().begin() + (index + size));
}

etm::Line::size_type etm::Line::iterator::getIndex() const {
    return index;
}

etm::Line::codepoint::codepoint() {
}
etm::Line::codepoint::codepoint(const string_t::const_iterator &start, const string_t::const_iterator &end):
    start(start), end(end)
{}
bool etm::Line::codepoint::operator==(value_type c) const {
    return *start == c;
}
bool etm::Line::codepoint::operator!=(value_type c) const {
    return *start != c;
}

etm::Line::size_type etm::Line::findDefactoSize(const string_t &string) {
    return findDefactoSize(string.begin(), string.end());
}

etm::Line::size_type etm::Line::findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end) {
    size_type size = 0;
    for (string_t::const_iterator it = start; it < end;) {
        if (ctrl::testStart(*it)) {
            it += ctrl::getJump() + 1;
        } else {
            it += utf8::test(*it);
            size++;
        }
    }
    return size; 
}

etm::Line::Line():
    defactoSize(0),
    newline(false),
    startSpace(false) {
}

etm::Line::size_type etm::Line::correctIndex(size_type index) {
    size_type i = 0;
    for (size_type cIndex = 0; cIndex < index;) {
        if (ctrl::testStart(string[i])) {
            i += ctrl::getJump();
        } else {
            i += utf8::test(string[i]);
            cIndex++;
        }
    }
    while (i < string.size() && ctrl::testStart(string[i])) {
        i += ctrl::getJump() + 1;
    }
    return i;
}
etm::Line::value_type &etm::Line::operator[](size_type index) {
    return string[correctIndex(index)];
}


etm::Line::iterator etm::Line::last() {
    // Account for if the last few bytes are control chars
    size_type index = string.size() - 1;
    while (ctrl::testEnd(string[index])) {
        index -= ctrl::getJump() + 1;
    }
    return iterator(this, index - utf8::lookbehind(string, index));
}

etm::Line::iterator etm::Line::begin() {
    return iterator(this, correctIndex(0));
}

etm::Line::size_type etm::Line::size() {
    return defactoSize;
}
void etm::Line::appendChar(const codepoint &c) {
    string.append(c.start, c.end);
    defactoSize++;
}
void etm::Line::appendChar(value_type chr) {
    string += chr;
    defactoSize++;
}
void etm::Line::insertChar(size_type index, const codepoint &c) {
    string.insert(string.begin() + correctIndex(index), c.start, c.end);
    defactoSize++;
}
void etm::Line::insertChar(size_type index, value_type chr) {
    string.insert(string.begin() + correctIndex(index), chr);
    defactoSize++;
}
void etm::Line::prependStr(const string_t &str) {
    string.insert(string.begin(), str.begin(), str.end());
    defactoSize += findDefactoSize(str);
}
void etm::Line::doErase(size_type index) {
    size_type lost = findDefactoSize(string.begin() + index, string.end());
    string.erase(index);
    defactoSize -= lost;
}
void etm::Line::erase(size_type index) {
    doErase(correctIndex(index));
}
void etm::Line::erase(const iterator &start) {
    doErase(start.getIndex());
}
void etm::Line::eraseChar(size_type index) {
    index = correctIndex(index);
    // Remove the entire codepoint
    string.erase(index, utf8::test(string[index]));
    defactoSize--;
}
void etm::Line::appendStr(const string_t &str) {
    string += str;
    defactoSize += findDefactoSize(str);
}
void etm::Line::appendOther(const Line &other) {
    // A line cannot have more than one newline -
    // it just wouldn't make sense :/
    newline |= other.newline;
    if (other.startSpace) {
        appendChar(' ');
    }
    string += other.string;
    defactoSize += other.defactoSize;
}
void etm::Line::popBack() {
    erase(last());
}

void etm::Line::appendControl(const string_t &val) {
    string += val;
}

void etm::Line::appendControlFromRange(const string_t &val, size_type index, size_type length) {
    string.append(val, index, length);
}

etm::Line::string_t etm::Line::substr(size_type startInc) {
    return string.substr(correctIndex(startInc));
}

void etm::Line::copyTo(string_t &str) {
    str.insert(str.end(), string.begin(), string.end());
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

etm::Line::value_type &etm::Line::getDejure(size_type index) {
    return string[index];
}
etm::Line::size_type etm::Line::dejureSize() {
    return string.size();
}

etm::Line::string_t &etm::Line::getString() {
    return string;
}
