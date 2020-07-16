#include "Line.h"

#include <iostream>
#include <cmath>

#include "env.h"

etm::Line::size_type etm::Line::findDefactoSize(const string_t &string) {
    return findDefactoSize(string.begin(), string.end());
}

etm::Line::size_type etm::Line::findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end) {
    size_type size = end - start;
    for (string_t::const_iterator it = start; it < end; ++it) {
        if (*it == env::CONTROL_CHAR) {
            it += env::CONTROL_BLOCK_SIZE;
            size -= env::CONTROL_BLOCK_SIZE;
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
    for (size_type cIndex = 0; cIndex < index; i++) {
        if (string[i] == env::CONTROL_CHAR) {
            i += env::CONTROL_BLOCK_SIZE;
        } else {
            cIndex++;
        }
    }
    while (i == env::CONTROL_CHAR) {
        i += env::CONTROL_BLOCK_SIZE + 1;
    }
    return i;
}

etm::Line::value_type &etm::Line::operator[](size_type index) {
    if (correctIndex(index) >= string.size()) {
        std::cout << "OUT OF BOUNDS!!" << std::endl;
        std::cout << "index = " << index << std::endl;
        std::cout << "correctIndex(index) = " << correctIndex(index) << std::endl;
        std::cout << "string.size() = " << string.size() << std::endl;
        exit(32);
    }
    return string[correctIndex(index)];
}
etm::Line::size_type etm::Line::size() {
    return defactoSize;
}
void etm::Line::append(value_type c) {
    string.push_back(c);
    defactoSize++;
}
void etm::Line::insert(size_type index, value_type c) {
    string.insert(string.begin() + correctIndex(index), c);
    defactoSize++;
}
void etm::Line::prependStr(const string_t &str) {
    string.insert(string.begin(), str.begin(), str.end());
    defactoSize += findDefactoSize(str);
}
void etm::Line::erase(size_type index) {
    index = correctIndex(index);
    size_type lost = findDefactoSize(string.begin() + index, string.end());
    string.erase(index);
    defactoSize -= lost;
}
void etm::Line::eraseChar(size_type index) {
    index = correctIndex(index);
    string.erase(index, 1);
    defactoSize--;
}
void etm::Line::appendStr(const string_t &str) {
    string += str;
    defactoSize += findDefactoSize(str);
}
void etm::Line::appendOther(const Line &other) {
    newline |= other.newline;
    if (other.startSpace) {
        append(value_type(' '));
    }
    defactoSize += other.defactoSize;
    appendStr(other.string);
}
void etm::Line::popBack() {
    string.pop_back();
    defactoSize--;
}

void etm::Line::appendControl(const string_t &val) {
    string += val;
}

etm::Line::string_t etm::Line::substr(size_type startInc) {
    return string.substr(correctIndex(startInc));
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

etm::Line::value_type etm::Line::getDejure(size_type index) {
    return string[index];
}
etm::Line::size_type etm::Line::dejureSize() {
    return string.size();
}
