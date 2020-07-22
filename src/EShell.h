#ifndef ETERMAL_ESHELL_H_INCLUDED
#define ETERMAL_ESHELL_H_INCLUDED

#include <string>

namespace etm {

    /**
    * Base interface for an Etermal backend.
    * Handles command processing.
    * @see Shell
    * @see ETerminal
    */
    class EShell {
    public:
        virtual ~EShell() = 0;

        /**
        * Send a command string to the shell to be processed.
        * Typically from user input.
        * @param [in] commandString The command string
        */
        virtual void input(const std::string &commandString) = 0;
    };
}

#endif
