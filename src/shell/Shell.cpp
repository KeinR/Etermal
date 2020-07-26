#include "Shell.h"

#include <iostream>

#include "../ETerminal.h"
#include "shellError.h"

static void makeLowercase(std::string &str);
static void defaultErrorCallback(const etm::shellError &error);
static std::string defaultNoCommandCallback(const std::string &command);

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
        << "ETERMAL::SHELL::ERROR:\n"
        << "location = " << error.location << "\n"
        << "severe = " << (error.severe ? "TRUE" : "FALSE") << "\n"
        << "message = \"" << error.message << "\"\n"
        << "---------------------------\n";
}

std::string defaultNoCommandCallback(const std::string &command) {
    return "\e[fd13400;Error\e[F: Command not found: " + command + "\n";
}

etm::Shell::Command::Command() {
}
etm::Shell::Command::Command(const ArgFilter &filter, const callback_t &callback):
    filter(filter), callback(callback) {
}

etm::Shell::Shell(): Shell(defaultErrorCallback) {
}
etm::Shell::Shell(const errCallback_t &callback):
    terminal(nullptr),
    flags(flag::none),
    prompt("\nuser@terminal ~\n$ "),
    commandId(0)
{
    setErrorCallback(callback);
    setNoCommandCallback(defaultNoCommandCallback);
}

void etm::Shell::setErrorCallback(const errCallback_t &callback) {
    if (callback) {
        errorCallback = callback;
    }
}
void etm::Shell::setNoCommandCallback(const noCommandCallback_t &callback) {
    if (callback) {
        noCommandCallback = callback;
    }
}

etm::Shell::errCallback_t etm::Shell::getErrorCallback() {
    return errorCallback;
}
etm::Shell::noCommandCallback_t etm::Shell::getNoCommandCallback() {
    return noCommandCallback;
}

void etm::Shell::setPrompt(const std::string &str) {
    prompt = str;
}
std::string etm::Shell::getPrompt() {
    return prompt;
}

bool etm::Shell::flagSet(flag::type f) {
    return (flags & f) == f;
}

void etm::Shell::setFlags(flag::type flags) {
    this->flags = flags;
}

void etm::Shell::setTerminal(ETerminal &terminal) {
    this->terminal = &terminal;
    prepTerminal();
}
void etm::Shell::input(const std::string &commandString) {
    // Defend against undefined behavior
    if (terminal == nullptr) {
        postError(
            "Shell::input(const std::string&)",
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
            Command &com = commands[loc->second];
            // Parse the commands
            Args args;
            std::string error;
            const bool failed = com.filter.filter(params, args, error);
            if (failed) {
                terminal->dispText(error);
                terminal->dispText(com.filter.getUsage());
                if (!com.filter.getErrorHandle().doFail()) {
                    terminal->flush();
                    com.callback(*this, *terminal, args);
                }
            } else {
                com.callback(*this, *terminal, args);
            }
        } else {
            terminal->dispText(noCommandCallback(params[0]));
        }
    }

    // Does flushing for us
    prepTerminal();
}

void etm::Shell::prepTerminal() {
    terminal->dispText(prompt);
    terminal->flush();
    terminal->setTakeInput(true);
}

void etm::Shell::doAlias(comid_t id, const std::string &name) {
    aliasMap.insert_or_assign(name, id);
}

etm::Shell::comid_t etm::Shell::addCommand(const std::string &name, const ArgFilter &filter, const callback_t &callback) {
    if (callback) {
        commandId++;
        commands.insert_or_assign(commandId, Command(filter, callback));
        doAlias(commandId, name);
        return commandId;
    } else {
        postError(
            "Shell::addCommand(const std::string&, const ArgFilter&, const callback_t&)",
            "The given callback is not callable. The command \"" + name + "\" will not be added",
            false
        );
        return 0;
    }
}
void etm::Shell::alias(comid_t id, const std::string &name) {
    if (commands.find(id) != commands.end()) {
        if (flagSet(flag::nocase)) {
            std::string lower(name);
            makeLowercase(lower);
            doAlias(id, lower);
        } else {
            doAlias(id, name);
        }
    } else {
        postError(
            "Shell::alias(comid_t,const std::string&)",
            "The given id of " + std::to_string(id) + " is invalid (doesn't exist)",
            false
        );
    }
}

void etm::Shell::alias(const std::string &name) {
    alias(commandId, name);
}

void etm::Shell::postError(const std::string &location, const std::string &message, bool severe) {
    errorCallback(shellError(location, message, severe));
}
