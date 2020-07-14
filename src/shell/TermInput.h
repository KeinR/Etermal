#ifndef ETERMAL_TERMINPUT_H_INCLUDED
#define ETERMAL_TERMINPUT_H_INCLUDED

#include <string>

namespace etm {
    class TermInput {
    public:
        virtual ~TermInput() = 0;
        virtual void terminalInput(const std::string &text) = 0;
    };
}

#endif
