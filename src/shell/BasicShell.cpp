#include "BasicShell.h"

#include <iostream>
#include <utility>

#include "../ETerminal.h"
#include "shellError.h"

static void makeLowercase(std::string &str);
static void defaultErrorCallback(const etm::shellError &error);
static std::string defaultNoCommandCallback(const std::string &command);
std::vector<std::string> chopString(const std::string &str);

static constexpr unsigned int MAX_COMMAND_HISTORY = 10;

void makeLowercase(std::string &str) {
    for (char &c : str) {
        if ('A' <= c && c <= 'Z') {
            c |= 0x20;
        }
    }
}

void defaultErrorCallback(const etm::shellError &error) {
    std::cerr
        << "---------------------------\n"
        << "ETERMAL::BASIC_SHELL::ERROR:\n"
        << "location = " << error.location << "\n"
        << "severe = " << (error.severe ? "TRUE" : "FALSE") << "\n"
        << "message = \"" << error.message << "\"\n"
        << "---------------------------\n";
}

std::string defaultNoCommandCallback(const std::string &command) {
    return "\e[fd13400;Error\e[F: Command not found: " + command + "\n";
}

etm::BasicShell::BasicShell(): BasicShell(defaultErrorCallback) {
}
etm::BasicShell::BasicShell(const errCallback_t &callback):
    terminal(nullptr),
    flags(flag::none),
    prompt("\nuser@terminal ~\n$ "),
    seekI(0)
{
    commandHistory.reserve(MAX_COMMAND_HISTORY);
    setErrorCallback(callback);
    setNoCommandCallback(defaultNoCommandCallback);
}

void etm::BasicShell::setErrorCallback(const errCallback_t &callback) {
    if (callback) {
        errorCallback = callback;
    }
}
void etm::BasicShell::setNoCommandCallback(const noCommandCallback_t &callback) {
    if (callback) {
        noCommandCallback = callback;
    }
}

etm::BasicShell::errCallback_t etm::BasicShell::getErrorCallback() {
    return errorCallback;
}
etm::BasicShell::noCommandCallback_t etm::BasicShell::getNoCommandCallback() {
    return noCommandCallback;
}

void etm::BasicShell::setPrompt(const std::string &str) {
    prompt = str;
}
std::string etm::BasicShell::getPrompt() {
    return prompt;
}

bool etm::BasicShell::flagSet(flag::type f) {
    return (flags & f) == f;
}

void etm::BasicShell::setFlags(flag::type flags) {
    this->flags = flags;
}

void etm::BasicShell::setTerminal(ETerminal &terminal) {
    this->terminal = &terminal;
    prepTerminal();
}
void etm::BasicShell::input(const std::string &commandString) {
    // Defend against undefined behavior
    if (terminal == nullptr) {
        postError(
            "BasicShell::input(const std::string&)",
            "Cannot recieve input, terminal is nullptr (not set)",
            false
        );
        return;
    }

    terminal->setTakeInput(false);

    std::vector<std::string> params;
    // If bypassing whitespace
    bool waiting = true;
    bool inString = false;
    for (char c : commandString) {
        if (c != ' ' || inString) {
            if (waiting) {
                params.emplace_back();
                waiting = false;
            }
            if (c == '\"') {
                inString = !inString;
            } else {
                params.back().push_back(c);
            }
        } else {
            waiting = true;
        }
    }
    if (params.size()) {
        if (flagSet(flag::nocase)) {
            makeLowercase(params[0]);
        }
        // Check the command exists
        aliasMap_t::iterator loc = aliasMap.find(params[0]);
        if (loc != aliasMap.end()) {
            commands[loc->second](chopString(commandString));
        } else {
            terminal->dispText(noCommandCallback(params[0]));
        }

        if (commandHistory.size() >= MAX_COMMAND_HISTORY) {
            commandHistory.erase(commandHistory.begin(), commandHistory.begin() + (commandHistory.size() - MAX_COMMAND_HISTORY + 1));
        }
        commandHistory.push_back(commandString);
        seekI = commandHistory.size();
    }

    // Does flushing for us
    prepTerminal();
}

void etm::BasicShell::setInput(const std::string &str) {
    terminal->clearInput();
    terminal->dispText(str);
    terminal->softFlush();
}

void etm::BasicShell::cursorUp() {
    if (commandHistory.size() > 0 && seekI != 0) {
        if (seekI == commandHistory.size()) {
            currentText = terminal->pollInput();
        }
        seekI--;
        setInput(commandHistory[seekI]);
    }
}
void etm::BasicShell::cursorDown() {
    if (commandHistory.size() > 0) {
        if (seekI < commandHistory.size() - 1) {
            seekI++;
            setInput(commandHistory[seekI]);
        } else {
            seekI = commandHistory.size();
            setInput(currentText);
            currentText.clear();
            currentText.shrink_to_fit();
        }
    }
}

void etm::BasicShell::prepTerminal() {
    terminal->dispText(prompt);
    terminal->flush();
    terminal->setTakeInput(true);
}

void etm::BasicShell::add(const std::string &name, const callback_t &callback) {
    if (callback) {
        commands.push_back(callback);
        alias(name);
    } else {
        postError(
            "BasicShell::add(const std::string&,const callback_t&)",
            "The given callback is not callable. The command \"" + name + "\" will not be added",
            false
        );
    }
}
void etm::BasicShell::alias(const std::string &name) {
    if (commands.size() > 0) {
        if (flagSet(flag::nocase)) {
            std::string lower(name);
            makeLowercase(lower);
            aliasMap[lower] = commands.size() - 1;
        } else {
            aliasMap[name] = commands.size() - 1;
        }
    } else {
        postError(
            "BasicShell::alias(const std::string&)",
            "There are no commands to alias",
            false
        );
    }
}

void etm::BasicShell::postError(const std::string &location, const std::string &message, bool severe) {
    errorCallback(shellError(location, message, severe));
}

std::vector<std::string> chopString(const std::string &str) {
    std::vector<std::string> result;

    for (std::string::size_type i = 0; i < str.size(); i++) {
        if (str[i] != ' ') {
            std::string block;
            bool inString = false;
            bool escaped = false;
            for (; i < str.size() && (str[i] != ' ' || inString); i++) {
                if (!escaped) {
                    if (str[i] == '\\') {
                        escaped = true;
                    } else if (str[i] == '"') {
                        if (inString) {
                            break;
                        } else {
                            inString = true;
                        }
                    } else {
                        block.push_back(str[i]);
                    }
                } else {
                    block.push_back(str[i]);
                }
            }
            result.push_back(block);
            result.back().shrink_to_fit();
        }
    }

    return result;
}
