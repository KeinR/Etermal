#ifndef ETERMAL_TERMINAL_H_INCLUDED
#define ETERMAL_TERMINAL_H_INCLUDED

#include <deque>
#include <memory>
#include <functional>

#include "util/enums.h"
#include "util/Timer.h"
// #include "gui/TextInput.h"
// #include "gui/ScrollBox.h"
#include "Resources.h"
#include "TextBuffer.h"
#include "gui/Rectangle.h"
#include "../ETerminal.h"
#include "Scroll.h"
#include "gui/Scrollbar.h"

namespace etm { 
    // ../shell/EShell
    class EShell;
    // ../shell/TermInput
    class TermInput;
}

/*
* Frontend for the shell
*/

namespace etm {
    class Terminal: public ETerminal {
    public:
        typedef std::deque<TermInput*> inputRequests_t;
        typedef std::function<void()> winActionCB_t;
    private:
        std::unique_ptr<Resources> resources;

        Model viewport;
        int maxWidth;
        int maxHeight;

        Scroll scroll;
        Scrollbar scrollbar;
        // Factor when doing mouse scroll
        float scrollSensitivity;
        // Scroll builds up until it reaches one char
        // height...
        // Aligns the scrolling to the text
        float scrollBuffer;

        TextBuffer display;
        Rectangle background;

        std::string displayBuffer; // Display
        std::string inputBuffer; // User input

        inputRequests_t inputRequests;

        bool focused;
        // Should the terminal take user input?
        bool takeInput;

        // Should the next char be escaped?
        bool escapeNext;

        Timer cursorBlink;

        EShell *shell;

        winActionCB_t windowSetCursorDefault;
        winActionCB_t windowSetCursorIBeam;
        // Is the mouse hovering over the terminal input area?
        bool hovering;

        void displayWelcome();

        void flushInputBuffer();
        void prepareInput();
        void doInputChar(char c);
        bool acceptInput();
        // Expects i to be pointing to the char preceeding the hex.
        // Reads a hex that starts on i and is 3 chars long.
        // i is set to the last character of the hex.
        // Will stop prematurely if the hex is truncated
        int readHexFromStr(std::string &str, std::string::size_type &i);
    public:

        // Initializes with the current context.
        // NOTE: THE DESIRED CONTEXT MUST BE CURRENT WHEN IT IS INITIALIZED!!!
        // FAILURE TO DO SO WILL RESULT IN BAD THIGNS HAPPENING
        Terminal();
        Terminal(Terminal &&other) = delete; // Temp
        Terminal &operator=(Terminal &&other) = delete;

        void setCursorDefault(const winActionCB_t &callback);
        void setCursorIBeam(const winActionCB_t &callback);
        void setHovering(bool value);

        void clear() override;

        void setBackgroundColor(const Color &color);
        void setTextColor(const Color &color);

        // Set the modifier for how much is scrolled on mouse
        // scroll.
        // negative values will result in inverted scroll.
        void setScrollSensitivity(float value);

        // The shell is where user input will be directed
        void setShell(EShell &shell);

        // Sets whether the terminal will process user input.
        // And send it to the shell.
        // Will still fulfill input requests
        void setTakeInput(bool value) override;

        void displayPrompt() override;

        // Each call adds `callback` to the input queue.
        // Each time input is entered, the callback is called
        // and removed.
        // Be careful of multible calls to this,
        // for you could become locked in.
        void requestInput(TermInput &callback) override;
        // Removes all instances of `callback` from queue
        void cancelInputRequest(TermInput *callback) override;
        // Deletes all input requests.
        // Only call if you know what you're doing.
        void clearInputRequests() override;

        // Inputs text to the display buffer.
        // Will not actually display until flushed.
        void dispText(const std::string &str) override;
        // Manually pushes (flushes) the display buffer to the display
        void flush() override;

        void setX(float x);
        void setY(float y);
        void setMaxWidth(float width);
        void setMaxHeight(float height);

        // Set font pixel size.
        // Note that this will resize the terminal,
        // however it will not become larger than the set
        // max width and height
        void setFontSize(unsigned int size);

        // Position elemnts according to coords
        void updatePosition();

        // Input events

        // All printable keys
        // inputChar/String aren't buffered.
        // Pushes data to input func upon encountering a newline.
        void inputChar(char c);
        void inputString(const std::string &text);

        // Backspace, enter, up, down, left, right keys
        void inputActionKey(actionKey key);
        // Only the y offset is relevent.
        // Value is difference from the last scroll
        // position
        void inputMouseScroll(float yOffset, float mouseX, float mouseY);
        // Viewport coordinates of the mouse
        // (model coords are used to calculate if mouse clicked)
        void inputMouseClick(bool isPressed, float mouseX, float mouseY);
        void inputMouseMove(float mouseX, float mouseY);

        // Renders to the current context
        void render();
    };
}

#endif
