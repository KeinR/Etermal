#ifndef ETERMAL_TERMINPUT_H_INCLUDED
#define ETERMAL_TERMINPUT_H_INCLUDED

#include <string>

namespace etm {

    /**
    * Interface class used to supply callbacks to an @ref ETerminal
    * @see ETerminal::requestInput(TermInput &callback)
    * @see ETerminal::cancelInputRequest(TermInput *callback)
    */
    class TermInput {
    public:
        virtual ~TermInput() = 0;

        /**
        * Recieve a UTF-8 encoded string if input from the terminal.
        * @param [in] text The input text
        * @see ETerminal
        */
        virtual void terminalInput(const std::string &text) = 0;
    };
}

#endif
