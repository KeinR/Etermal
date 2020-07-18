#ifndef ETERMAL_ETERMINAL_H_INCLUDED
#define ETERMAL_ETERMINAL_H_INCLUDED

#include <string>

namespace etm { class TermInput; }

namespace etm {
    class ETerminal {
    public:
        virtual ~ETerminal() = 0;
        // Does the shell want any input right now?
        // Will still fulfill input requests (requestInput(...))
        virtual void setTakeInput(bool value) = 0;

        // Clears the screen
        virtual void clear() = 0;

        // Each call adds `callback` to the input queue.
        // Each time input is entered, the callback is called
        // and removed.
        // Be careful of multible calls to this,
        // for you could become locked in.
        virtual void requestInput(TermInput &callback) = 0;
        // Removes all instances of `callback` from queue
        virtual void cancelInputRequest(TermInput *callback) = 0;
        // Deletes all input requests.
        // Only call if you know what you're doing.
        virtual void clearInputRequests() = 0;
        virtual void dispText(const std::string &str) = 0;
        virtual void flush() = 0;
    };
}

#endif
