#include "Shell.h"

#include "ETerminal.h"

static void makeLowercase(std::string &str) {
    for (char &c : str) {
        if ('A' <= c && c <= 'Z') {
            c |= 0x20;
        }
    }
}

etm::Shell::Command::Command() {
}
etm::Shell::Command::Command(const ArgFilter &filter, const callback_t &callback):
    filter(filter), callback(callback) {
}

etm::Shell::Shell(): terminal(nullptr), flags(flag::none) {
}

bool etm::Shell::flagSet(flag::type f) {
    return (flags & f) == f;
}

void etm::Shell::setFlags(flag::type flags) {
    this->flags = flags;
}

void etm::Shell::setTerminal(ETerminal &terminal) {
    this->terminal = &terminal;
    terminal.displayPrompt();
    terminal.setTakeInput(true);
}
void etm::Shell::input(const std::string &commandString) {
    terminal->setTakeInput(false);
    std::vector<std::string> params;
    bool waiting = true;
    bool inString = false;
    for (char c : commandString) {
        if (c != ' ' || inString) {
            if (waiting) {
                params.emplace_back();
                waiting = false;
            }
            if (c == '\"') {
                if (inString) {
                    waiting = true;
                }
                inString = !inString;
            } else {
                params.back().push_back(c);
            }
        } else if (!waiting) {
            waiting = true;
        }
    }
    if (params.size()) {
        if (flagSet(flag::nocase)) {
            makeLowercase(params[0]);
        }
        aliasMap_t::iterator loc = aliasMap.find(params[0]);
        if (loc != aliasMap.end()) {
            Command &com = commands[loc->second];
            Args args;
            std::string error = com.filter.filter(params, args);
            com.callback(*this, *terminal, args);
        } else {
            terminal->dispText("Command not found: " + params[0] + "\n");
        }
    }

    terminal->displayPrompt();
    terminal->flush(); // Make sure it's flushed
    terminal->setTakeInput(true);
}

void etm::Shell::addCommand(const std::string &name, const ArgFilter &filter, const callback_t &callback) {
    commands.emplace_back(filter, callback);
    alias(name);
}
void etm::Shell::alias(const std::string &name) {
    if (flagSet(flag::nocase)) {
        std::string lower = name;
        makeLowercase(lower);
        aliasMap[lower] = commands.size() - 1;
    } else {
        aliasMap[name] = commands.size() - 1;
    }
}

