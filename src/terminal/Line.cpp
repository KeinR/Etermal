#include "Line.h"

#include <iostream>
#include <cmath>

#include "env.h"


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
    index -= utf8::lookbehind(parent->getString(), index);
    // "If pos is equal to the string length, the
    // function returns a reference to the null character
    // that follows the last character in the string (which
    // should not be modified)."
    // - http://www.cplusplus.com/reference/string/basic_string/operator[]/
    // 
    // We're fine not doing bounds checks, so long as the caller
    // always checks valid() before de/incrementing, and env::CONTROL_CHAR_END
    // and env::CONTROL_CHAR_START are never set to the null character
    while (parent->getDejure(index) == env::CONTROL_CHAR_END) {
        index -= env::CONTROL_BLOCK_SIZE + 1;
    }
    index -= utf8::lookbehind(parent->getString(), index);
}
void etm::Line::iterator::operator++() {
    index += utf8::test(parent->getDejure(index));
    while (parent->getDejure(index) == env::CONTROL_CHAR_START) {
        index += env::CONTROL_BLOCK_SIZE + 1;
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
bool etm::Line::codepoint::operator==(char c) const {
    return *start == c;
}
bool etm::Line::codepoint::operator!=(char c) const {
    return *start != c;
}

etm::Line::size_type etm::Line::findDefactoSize(const string_t &string) {
    return findDefactoSize(string.begin(), string.end());
}

etm::Line::size_type etm::Line::findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end) {
    size_type size = 0;
    for (string_t::const_iterator it = start; it < end;) {
        if (*it == env::CONTROL_CHAR_START) {
            it += env::CONTROL_BLOCK_SIZE + 1;
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
        if (string[i] == env::CONTROL_CHAR_START) {
            i += env::CONTROL_BLOCK_SIZE;
        } else {
            i += utf8::test(string[i]);
            cIndex++;
        }
    }
    while (i < string.size() && string[i] == env::CONTROL_CHAR_START) {
        i += env::CONTROL_BLOCK_SIZE + 1;
    }
    return i;
}
etm::Line::value_type &etm::Line::operator[](size_type index) {
    if (correctIndex(index) >= string.size()) {
        std::cerr << "OUT OF BOUNDS!!" << std::endl;
        std::cerr << "index = " << index << std::endl;
        std::cerr << "correctIndex(index) = " << correctIndex(index) << std::endl;
        std::cerr << "string.size() = " << string.size() << std::endl;
        std::cout << "string == \"" << string << "\"" << std::endl;
    }
    return string[correctIndex(index)];
}


etm::Line::iterator etm::Line::last() {
    size_type index = string.size() - 1;
    while (string[index] == env::CONTROL_CHAR_END) {
        index -= env::CONTROL_BLOCK_SIZE + 1;
    }
    return iterator(this, index);
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
    string.erase(index, utf8::test(string[index]));
    defactoSize--;
}
void etm::Line::appendStr(const string_t &str) {
    string += str;
    defactoSize += findDefactoSize(str);
}
void etm::Line::appendOther(const Line &other) {
    newline |= other.newline;
    if (other.startSpace) {
        appendChar(' ');
    }
    string += other.string;
    defactoSize += other.defactoSize;
}
void etm::Line::popBack() {
    std::cout << "break" << std::endl;
    doErase(string.size() - 1 - utf8::lookbehind(string, string.size() - 1));
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
