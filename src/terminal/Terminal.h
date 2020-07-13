#ifndef ETERMAL_TERMINAL_H_INCLUDED
#define ETERMAL_TERMINAL_H_INCLUDED

#include <deque>
#include <memory>

#include "util/enums.h"
#include "util/Timer.h"
// #include "gui/TextInput.h"
// #include "gui/ScrollBox.h"
#include "Resources.h"
#include "TextBuffer.h"
#include "gui/Rectangle.h"

namespace etm { 
    // ../shell/Shell
    class Shell;
    // TermInput
    class TermInput;
}

/*
* Frontend for the shell
*/

namespace etm {
    class Terminal {
    public:
        typedef std::deque<TermInput*> inputRequests_t;
        typedef bool(*flushFunc_t)(char);
    private:
        std::unique_ptr<Resources> resources;

        Model viewport;
        int maxWidth;
        int maxHeight;

        // std::string line1text; // TEMP
        // Text testText; // TEMP
        // Image testImage; // TEMP
        // std::vector<Text> lines;
        // std::string textBuffer;
        // Text text; // temporary measure
        // std::vector<Character> buffer;
        // ScrollBox output;
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

        Shell *shell;

        void displayWelcome();
        void displayPrompt();

        void flushInputBuffer();
        void prepareInput();
        void doInputChar(char c);
        void deleteLastChar();
    public:

        // Initializes with the current context.
        // NOTE: THE DESIRED CONTEXT MUST BE CURRENT WHEN IT IS INITIALIZED!!!
        // FAILURE TO DO SO WILL RESULT IN BAD THIGNS HAPPENING
        Terminal();
        Terminal(Terminal &&other) = default;
        Terminal &operator=(Terminal &&other) = default;

        // The shell is where user input will be directed
        void setShell(Shell &shell);

        // Sets whether the terminal will process user input.
        // If this is set to false, all user input is discarded.
        // If it's true, user input will be shown as it's typed.
        // If true, calls to `flush()` will be denied.
        void setTakeInput(bool value);

        // Each call adds `callback` to the input queue.
        // Each time input is entered, the callback is called
        // and removed.
        // Be careful of multible calls to this,
        // for you could become locked in.
        void requestInput(TermInput &callback);
        // Removes all instances of `callback` from queue
        void cancelInputRequest(TermInput *callback);
        // Deletes all input requests.
        // Only call if you know what you're doing.
        void clearInputRequests();

        // Inputs text to the display buffer.
        // Will not actually display until flushed.
        void dispText(const std::string &str);
        // Manually pushes (flushes) the display buffer to the display
        void flush();
        // // Sets whether the terminal should automatically flush
        // // the dispaly buffer
        // void setAutoFlush(bool val);

        void setX(float x);
        void setY(float y);
        void setMaxWidth(float width);
        void setMaxHeight(float height);

        // Set font pixel size.
        // Note that this will resize the terminal,
        // however it will not become larger than the set
        // max width and height
        // void setFontSize(int size);

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
        void inputMouseScroll(float yOffset);
        // Viewport coordinates of the mouse
        // (model coords are used to calculate if mouse clicked)
        void inputMouseClick(bool isPressed, float mouseX, float mouseY);
        void inputMouseMove(float mouseX, float mouseY);

        // Renders to the current context
        void render();
    };
}

#endif
