#ifndef ETERMAL_ETERMINAL_H_INCLUDED
#define ETERMAL_ETERMINAL_H_INCLUDED

#include <string>

namespace etm { class TermInput; }

namespace etm {

    /**
    * Base interface for an Etermal frontend.
    * @see Terminal
    * @see EShell
    */
    class ETerminal {
    public:
        virtual ~ETerminal() = 0;
        /**
        * Sets whether the shell wants input.
        * If this is false and there are no queued input
        * requests, the terminal will ignore input.
        * @param [in] value Whether the shell wants input
        * @see requestInput(TermInput &callback)
        * @see EShell
        */
        virtual void setTakeInput(bool value) = 0;

        /**
        * Clears the screen.
        * Causes all caches and other data
        * related to the text and screen to be cleared.
        * @warning Will also clear the display buffer [@ref dispText(const std::string &str)]
        * @see dispText(const std::string &str)
        * @see flush()
        */
        virtual void clear() = 0;

        /**
        * Each call adds `callback` to the input queue.
        * Each time input is entered, the callback is called and removed.
        * Be careful of multible calls to this, for you could become locked in.
        * @param [in] callback The callback to recieve the input text as a UTF-8 encoded string
        * @see cancelInputRequest(TermInput *callback)
        */
        virtual void requestInput(TermInput &callback) = 0;

        /**
        * Removes all instances of a callback from the input queue.
        * @param [in] callback pointer to the callback to be removed
        * @see requestInput(TermInput &callback)
        */
        virtual void cancelInputRequest(TermInput *callback) = 0;

        /**
        * Deletes all input requests.
        * @see requestInput(TermInput &callback)
        * @see cancelInputRequest(TermInput *callback)
        */
        virtual void clearInputRequests() = 0;

        /**
        * Append text to the display buffer.
        * The buffer will not be displayed until flush()
        * is called.
        * @param [in] str UTF-8 encoded string to be appended to the display buffer
        * @see flush()
        */
        virtual void dispText(const std::string &str) = 0;

        /**
        * Pushes the display buffer to the display.
        * @see dispText(const std::string &str)
        */
        virtual void flush() = 0;
    };
}

#endif
