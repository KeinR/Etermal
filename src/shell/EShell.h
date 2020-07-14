#ifndef ETERMAL_ESHELL_H_INCLUDED
#define ETERMAL_ESHELL_H_INCLUDED

#include <string>

/*
* Program backend, handles command processing, etc.
*/

namespace etm {
    class EShell {
    public:
        virtual ~EShell() = 0;
        // Send command string to be processed, typically
        // from user input.
        virtual void input(const std::string &commandString) = 0;
    };
}

#endif
