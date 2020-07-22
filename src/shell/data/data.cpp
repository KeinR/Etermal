#include "data.h"

#include <exception>

/**
* Does a very dumb parse for bool.
* The point is not accuracy, but rather to get some
* reasonable value.
* @param [in] str The string to parse
* @return The boolean value
*/
static bool parseBool(const std::string &str);

bool parseBool(const std::string &str) {
    typedef std::string::size_type size;
    size start = 0;
    size end = str.size() - 1;
    for (; start < str.size() - 1 && str[start] == ' '; start++);
    for (; end > 0 && str[end] == ' '; end--);
    if (end - start == 1) {
        return str[start] == '1';
    }

    std::string test = str.substr(start, end - start + 1);
    for (char &c : test) {
        if ('A' <= c && c <= 'Z') {
            c |= 0x20;
        }
    }
    return test == "true";
}

etm::data::String::String(const std::string &str) noexcept: str(str) {
}
std::string etm::data::String::getString() noexcept {
    return str;
}
bool etm::data::String::getBool() noexcept {
    return parseBool(str);
}
int etm::data::String::getInt() noexcept {
    try {
        return std::stoi(str);
    } catch (std::exception &e) {
        return 0;
    }
}
float etm::data::String::getFloat() noexcept {
    try {
        return std::stof(str);
    } catch (std::exception &e) {
        return 0.0f;
    }
}


etm::data::Boolean::Boolean(bool flag) noexcept: flag(flag) {
}
std::string etm::data::Boolean::getString() noexcept {
    return flag ? "true" : "false";
}
bool etm::data::Boolean::getBool() noexcept {
    return flag;
}
int etm::data::Boolean::getInt() noexcept {
    return static_cast<int>(flag);
}
float etm::data::Boolean::getFloat() noexcept {
    return static_cast<float>(flag);
}


etm::data::Integer::Integer(int value) noexcept: value(value) {
}
std::string etm::data::Integer::getString() noexcept {
    return std::to_string(value);
}
bool etm::data::Integer::getBool() noexcept {
    return static_cast<bool>(value);
}
int etm::data::Integer::getInt() noexcept {
    return value;
}
float etm::data::Integer::getFloat() noexcept {
    return static_cast<float>(value);
}


etm::data::Float::Float(float value) noexcept: value(value) {
}
std::string etm::data::Float::getString() noexcept {
    return std::to_string(value);
}
bool etm::data::Float::getBool() noexcept {
    return static_cast<bool>(value);
}
int etm::data::Float::getInt() noexcept {
    return static_cast<int>(value);
}
float etm::data::Float::getFloat() noexcept {
    return value;
}
