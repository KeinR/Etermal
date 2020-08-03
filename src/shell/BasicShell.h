#ifndef ETERMAL_BASICSHELL_H_INCLUDED
#define ETERMAL_BASICSHELL_H_INCLUDED

#include <functional>
#include <vector>
#include <string>
#include <map>

#include "../EShell.h"

namespace etm {
    // ETerminal
    class ETerminal;
    // shellError
    class shellError;
}

namespace etm {

    /**
    * Built in implementation of @ref EShell.
    * Basically just a copy of @ref Shell that has a bunch
    * of stuff ripped out of it.
    * @see Terminal
    */
    class BasicShell: public EShell {
    public:
        /// Parameter type given to command callbacks
        typedef const std::vector<std::string>& param_t;

        /// Command callback.
        /// Called when a command is invoked.
        typedef std::function<void(param_t args)> callback_t;

        /// The command string type
        typedef std::string key_type;
        /// Type used for the command container
        typedef std::vector<callback_t> commands_t;
        /// Error callback.
        /// Called when an error is detected.
        typedef std::function<void(const shellError&)> errCallback_t;
        /// No-command callback.
        /// Return value should the be text to display to the user.
        /// The parameter is the command that was given.
        typedef std::function<std::string(const std::string&)> noCommandCallback_t;

        /**
        * Contains various shell flags
        */
        struct flag {
            /// The general flag datatype
            typedef int type;
            /// Flag for no flags
            static constexpr type none = 0;
            /// If set, all given aliases will be converted to lowercase,
            /// and so will the invokation commands (first command given,
            /// like in say `g++ ...`, "g++" is the invokation command)
            static constexpr type nocase = 1 << 0;
        };

    private:
        /// Type used for the alias map
        typedef std::map<key_type, commands_t::size_type> aliasMap_t;

        /// Called when functional errors are encountered
        /// @see setErrorCallback(const errCallback_t &callback)
        errCallback_t errorCallback;
        /// Called when the given command could not be found,
        /// returns the text to display to the user
        /// @see setNoCommandCallback(const noCommandCallback_t &callback)
        noCommandCallback_t noCommandCallback;
        /// Terminal frontend
        /// @see EShell
        /// @see Terminal
        ETerminal *terminal;
        /// Invokable commands
        /// @see addCommand(const std::string &name, const ArgFilter &filter, const callback_t &callback)
        commands_t commands;
        /// Command aliases, all point to a nameless command in @ref commands
        /// @see alias(const std::string &name)
        aliasMap_t aliasMap;
        /// Flags
        /// @see setFlags(flag::type flags)
        flag::type flags;
        /// Printed to screen after each command invokation is complete
        /// @see setPrompt(const std::string &str)
        std::string prompt;
        /// The shell's command history.
        /// Capped at an arbitrary number (small)
        std::vector<std::string> commandHistory;
        /// Stored text so that seeking operations can
        /// return to what the user was preveously typing.
        std::string currentText;
        /// Seek index for command history
        std::vector<std::string>::size_type seekI;

        /**
        * Checks if a flag is set.
        * @param [in] f The flag value
        * @return `true` if the flag is set
        */
        bool flagSet(flag::type f);
        /**
        * Prepare the terminal for input.
        */
        void prepTerminal();

        /**
        * Replaces the current user input.
        * @param [in] str The string to replace with
        */
        void setInput(const std::string &str);

    public:
        /**
        * Construct a shell.
        */
        BasicShell();
        /**
        * Construct a shell with an error callback.
        * No errors are set in the constructor ATM,
        * but it may still be a good idea to assure
        * that errors get processed correctly.
        * @note The error callback isn't set if it's not callable
        * @param [in] callback A callable callback
        * @see setErrorCallback(const errCallback_t &callback)
        */
        BasicShell(const errCallback_t &callback);

        /**
        * Set the callback that will be called whenever
        * a `shellError` is set.
        * @note The callback isn't set if it's not callable
        * @param [in] callback A callable callback
        * @see errCallback_t
        * @see getErrorCallback()
        */
        void setErrorCallback(const errCallback_t &callback);
        /**
        * Callback called when a given command could not be found.
        * @note The callback isn't set if it's not callable
        * @param [in] callback A callable callback
        * @see noCommandCallback_t
        * @see getNoCommandCallback()
        */
        void setNoCommandCallback(const noCommandCallback_t &callback);
        /**
        * Gets the current error callback.
        * It will be callable.
        * @return The callback
        * @see setErrorCallback(const errCallback_t &callback)
        */
        errCallback_t getErrorCallback();
        /**
        * Gets the current no-command callback.
        * It will be callable.
        * @return The callback
        * @see setNoCommandCallback(const noCommandCallback_t &callback)
        */
        noCommandCallback_t getNoCommandCallback();

        /**
        * Set the text that's displayed in the terminal
        * when the shell is taking user input.
        * @param [in] str UTF-8 encoded string to display
        * @see getPrompt()
        * @see input(const std::string &commandString)
        * @see setTerminal(ETerminal &terminal)
        */
        void setPrompt(const std::string &str);
        /**
        * Gets the current prompt.
        * @return The promt as a UTF-8 encoded string
        * @see setPrompt(const std::string &str)
        */
        std::string getPrompt();

        /**
        * Set terminal flags.
        * @param [in] flags The flags to set
        * @see flag
        */
        void setFlags(flag::type flags);
        /**
        * Set the program frontend.
        * The shell will fail at areas
        * [ex. @ref input(const std::string &commandString)]
        * where it needs to display text to the terminal.
        * @param [in] terminal The terminal frontend
        */
        void setTerminal(ETerminal &terminal);

        void input(const std::string &commandString) override;
        void cursorUp() override;
        void cursorDown() override;

        /**
        * Add a new command to the BasicShell.
        * @note If the parameter `callback` is not callable,
        * an error is set and the command is not added.
        * @param [in] name The initial alias for the command
        * @param [in] callback A callable callback for the command
        * @see alias(const std::string &name)
        * @see input(const std::string &commandString)
        */
        void add(const std::string &name, const callback_t &callback);

        /**
        * Associate another name with the @e last added command.
        * @note An error is set if there aren't any commands
        * @note If `name` already exists as an alias to some other
        * command, that alias gets re-pointed to the last command.
        * In other words, this function overwrites any previous alias
        * of the same name.
        * @param [in] name The alias
        */
        void alias(const std::string &name);

        /**
        * Send an error to the error callback.
        * The given parameters are packaged into a @ref shellError object.
        * @param [in] location The location at which the error was detected
        * @param [in] message The error message
        * @param [in] severe Whether the error is severe or not
        */
        void postError(const std::string &location, const std::string &message, bool severe);
    };
}

#endif
