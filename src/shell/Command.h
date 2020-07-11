#ifndef ETERMAL_COMMAND_H_INCLUDED
#define ETERMAL_COMMAND_H_INCLUDED

#include <functional>
#include <regex>

class Command {
public:
    typedef std::function<void()> callback_t;
private:
    callback_t callback;
    std::regex regex;
public:
    Command();
};

#endif
