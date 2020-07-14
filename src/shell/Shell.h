#ifndef ETERMAL_Shell_H_INCLUDED
#define ETERMAL_Shell_H_INCLUDED

#include <functional>
#include <map>

#include "EShell.h"
#include "ArgFilter.h"

namespace etm {
    // Args
    class Args;
    // ETerminal
    class ETerminal;
}

namespace etm {
    class Shell: public EShell {
    public:
        typedef std::string key_type;
        typedef std::function<void(Shell&,ETerminal&,Args&)> callback_t;

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

        ETerminal *terminal;
        commands_t commands;
        aliasMap_t aliasMap;
        flag::type flags;

        // Returns true if a flag is set
        bool flagSet(flag::type f);
    public:
        Shell();
        Shell(Shell &&other) = default;
        // Overwrites previous flags
        void setFlags(flag::type flags);
        void setTerminal(ETerminal &terminal);
        void input(const std::string &commandString) override;

        void addCommand(const std::string &name, const ArgFilter &filter, const callback_t &callback);
        // Associate another (given) name with the last added command.
        // Will throw an `std::out_of_range` exception if there are no commands.
        void alias(const std::string &name);
    };
}

#endif
