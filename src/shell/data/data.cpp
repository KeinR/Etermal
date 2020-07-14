#include "data.h"

#include <exception>
#include <limits>

// Does not check very hard for errors
static bool parseBool(const std::string &str) {
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
    return Integer(str).getInt();
}
float etm::data::String::getFloat() noexcept {
    return Float(str).getFloat();
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


etm::data::Integer::Integer(const std::string &source) noexcept {
    try {
        value = std::stoi(source);
        parseFailed = false;
    } catch (std::exception &e) {
        value = 0;
        parseFailed = true;
    }
}
bool etm::data::Integer::failed() noexcept {
    return parseFailed;
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


etm::data::Float::Float(const std::string &source) noexcept {
    try {
        value = std::stof(source);
        parseFailed = false;
    } catch (std::exception &e) {
        value = 0.0f;
        parseFailed = true;
    }
}
bool etm::data::Float::failed() noexcept {
    return parseFailed;
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
