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

        /**
        * Called when the user attempts to move the cursor @e up
        * when it's already at max height; think command
        * history...
        */
        virtual void cursorUp() = 0;
        /**
        * Called when the user attempts to move the cursor @e down
        * when it's already at max height; think command
        * history...
        */
        virtual void cursorDown() = 0;
    };
}

#endif
