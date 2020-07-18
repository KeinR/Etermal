#ifndef ETERMAL_Shell_H_INCLUDED
#define ETERMAL_Shell_H_INCLUDED

#include <functional>
#include <map>

#include "../EShell.h"
#include "ArgFilter.h"

namespace etm {
    // Args
    class Args;
    // ETerminal
    class ETerminal;
    // shellError
    class shellError;
}

namespace etm {
    class Shell: public EShell {
    public:
        typedef std::string key_type;
        typedef std::function<void(Shell&,ETerminal&,Args&)> callback_t;
        typedef std::function<void(const shellError&)> errCallback_t;
        // Return is the text to display to the user, param is the
        // command that was given
        typedef std::function<std::string(const std::string&)> noCommandCallback_t;

        struct flag {
            typedef int type;
            static constexpr type none = 0; // No flags
            // If set, all given aliases will be converted to lowercase,
            // and so will the invokation commands (first command given,
            // like in say `g++ ...`, "g++" is the invokation command)
            static constexpr type nocase = 1 << 0;
        };

    private:
        struct Command {
            ArgFilter filter;
            callback_t callback;
            Command();
            Command(const ArgFilter &filter, const callback_t &callback);
        };

        typedef std::vector<Command> commands_t;
        typedef std::map<std::string, commands_t::size_type> aliasMap_t;

        // Called when functional errors are encountered
        errCallback_t errorCallback;
        // Called when the given command could not be found,
        // returns the string to dispaly to the user
        noCommandCallback_t noCommandCallback;
        // Terminal frontend
        ETerminal *terminal;
        // Invokable commands
        commands_t commands;
        // Command aliases, all point to a nameless command
        aliasMap_t aliasMap;
        // Settings
        flag::type flags;
        // Printed to screen after each command invokation is complete
        std::string prompt;

        // Returns true if a flag is set
        bool flagSet(flag::type f);
        void prepTerminal();
    public:
        Shell();
        // With callback.
        // Note that as with setErrorCallback(...),
        // it denies callbacks that aren't callable.
        Shell(const errCallback_t &callback);
        Shell(Shell &&other) = delete; // temp
        // Set the callback that will be called whenever
        // a `shellError` is raised.
        // Denies callbacks that aren't callable.
        void setErrorCallback(const errCallback_t &callback);
        // Callback called when a given command could not be found.
        // Return is the text to display to the user, param is the
        // command that was given.
        // Does not set callback if it isn't callable.
        void setNoCommandCallback(const noCommandCallback_t &callback);
        // Gets copies of the corresponding callbacks
        errCallback_t getErrorCallback();
        noCommandCallback_t getNoCommandCallback();

        // Gets/sets the text that's displayed after
        // each command invokation
        void setPrompt(const std::string &str);
        std::string getPrompt();

        // Overwrites previous flags
        void setFlags(flag::type flags);
        void setTerminal(ETerminal &terminal);
        void input(const std::string &commandString) override;

        void addCommand(const std::string &name, const ArgFilter &filter, const callback_t &callback);
        // Associate another (given) name with the last added command.
        // Will throw an `std::out_of_range` exception if there are no commands.
        void alias(const std::string &name);

        // Send an error to the error callback
        void postError(const std::string &location, const std::string &message, bool severe);
    };
}

#endif
