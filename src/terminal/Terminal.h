#ifndef ETERMAL_TERMINAL_H_INCLUDED
#define ETERMAL_TERMINAL_H_INCLUDED

#include <deque>
#include <memory>
#include <functional>

#include "util/enums.h"
#include "util/Timer.h"
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
    // util/termError
    class termError;
}

namespace etm {

    /**
    * Build-in implementation of the @ref ETerminal frontend.
    */
    class Terminal: public ETerminal {
    public:
        /// Callback for a window action callback.
        /// Used to allow user to supply their own procedures
        /// for interfacing with the window.
        typedef std::function<void()> winActionCB_t;
        /// Error callback function.
        /// @param [in] error An object with error information
        typedef std::function<void(const termError &error)> errCallback_t;
    private:
        typedef std::deque<TermInput*> inputRequests_t;

        errCallback_t errorCallback;

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

        // Is the mouse dragging?
        bool dragging;
        // Coordinates of the last drag location
        float dragX;
        float dragY;

        void displayWelcome();

        // Pushes input to recievers (shell, etc)
        void pushInput(const std::string &input);
        void prepareInput();
        void doInputChar(const Line::codepoint &c);
        bool acceptInput();
        // Expects i to be pointing to the char preceeding the hex.
        // Reads a hex that starts on i and is 3 chars long.
        // i is set to the last character of the hex.
        // Will stop prematurely if the hex is truncated
        int readHexFromStr(std::string &str, std::string::size_type &i);
        // Maps screen coordinates to text coordinates
        void mapCoords(float x, float y, TextBuffer::lines_number_t &row, TextBuffer::line_index_t &column);
    public:

        /**
        * Creates a Terminal.
        * @note If an error occurred during initialization, the default callback will be called.
        * @warning The terminal will be tied to the current OpenGL context.
        * @todo Late initialization
        */
        Terminal();

        /**
        * Creates a Terminal with a custom error callback.
        * 
        * It's important to specify how you want errors handled on creation,
        * as errors can occur during initialization.
        * @warning The terminal will be tied to the current OpenGL context.
        * @param [in] errorCallback The error callback
        * @todo Late initialization
        * @see setErrorCallback(const errCallback_t &callback)
        */
        Terminal(const errCallback_t &errorCallback);
        Terminal(Terminal &&other) = delete; // Temp
        Terminal &operator=(Terminal &&other) = delete;

        /**
        * Pass an error to the current error callback.
        * @param [in] error The error object
        * @see setErrorCallback(const errCallback_t &callback)
        */
        void postError(const termError &error);

        /**
        * Set the cursor default callback.
        * 
        * This is called whenever the terminal would like to
        * set the cursor to its default state.
        * 
        * The callback does not have to be callable.
        * @param [in] callback The callback
        * @see setCursorIBeam(const winActionCB_t &callback)
        */
        void setCursorDefault(const winActionCB_t &callback);

        /**
        * Set the cursor i-beam callback.
        * 
        * This is called whenever the terminal would like to
        * set the cursor to i-beam (aka text input-mode).
        * 
        * The callback does not have to be callable.
        * @param [in] callback The callback
        * @see setCursorDefault(const winActionCB_t &callback)
        */
        void setCursorIBeam(const winActionCB_t &callback);

        /**
        * Set whether the mouse is hovering over the terminal.
        * 
        * If set to true and `hovering` is false, will call
        * the callback set by @ref setCursorIBeam(const winActionCB_t &callback)
        * (if it's callable), and vice versa with the callback set by
        * @ref setCursorDefault(const winActionCB_t &callback).
        * @param [in] callback The callback
        * @see setCursorDefault(const winActionCB_t &callback)
        * @see setCursorIBeam(const winActionCB_t &callback)
        */
        void setHovering(bool value);

        /**
        * Sets the global error callback for this Terminal.
        * All errors will be directed to this function.
        * @note `callback` must be callable. If not, the error callback isn't changed
        * @param [in] callback A new callable error callback
        * @see postError(const termError &error)
        */
        void setErrorCallback(const errCallback_t &callback);

        void clear() override;

        /**
        * Sets the background color of the terminal.
        * This also sets the default background color of text.
        * @param [in] color The color
        * @see setTextColor(const Color &color)
        */
        void setBackgroundColor(const Color &color);

        /**
        * Sets the default foreground color of printed text.
        * @param [in] color The color
        * @see setBackgroundColor(const Color &color)
        */
        void setTextColor(const Color &color);

        /**
        * Sets the mouse scroll sensitivity.
        * @note Negative values will result in inverted scroll.
        * @param [in] value The sensitivity
        */
        void setScrollSensitivity(float value);

        /**
        * Sets the shell to which user input should be directed.
        * @note If this isn't set and the terminal recieves input,
        * an error will be set.
        * @param [in] shell The shell object
        * @see EShell
        */
        void setShell(EShell &shell);

        // Sets whether the terminal will process user input.
        // And send it to the shell.
        // Will still fulfill input requests
        void setTakeInput(bool value) override;

        void requestInput(TermInput &callback) override;
        void cancelInputRequest(TermInput *callback) override;
        void clearInputRequests() override;

        void dispText(const std::string &str) override;
        void flush() override;

        /**
        * Set the x coordinate of the top-right corner of the terminal.
        * @param [in] x The x coordinate
        * @see setY(float y)
        */
        void setX(float x);

        /**
        * Set the y coordinate of the top-right corner of the terminal.
        * @param [in] y The y coordinate
        * @see setX(float x)
        */
        void setY(float y);

        void setMaxWidth(float width);
        void setMaxHeight(float height);

        /**
        * Sets the font pixel size.
        * @param [in] size The pixel size
        */
        void setFontSize(unsigned int size);

        /**
        * Set the maximum number of lines.
        * If the limit is exceed, the oldest lines will be removed.
        * @param [in] count number of lines
        */
        void setMaxLines(TextBuffer::lines_number_t count);

        /**
        * Update element positioning.
        */
        void updatePosition();

        // Input events

        /**
        * Input a char to the terminal, typically due to
        * user input.
        * @note Not buffered
        * @param [in] codepoint A Unicode codepoint
        * @see inputChar(const Line::codepoint &c)
        * @see inputString(const std::string &text)
        */
        void inputChar(unsigned int codepoint);

        /**
        * Input a char to the terminal, typically due to
        * user input.
        * This function, however, takes an encoded codepoint.
        * @note Not buffered
        * @param [in] codepoint The codepoint
        * @see Line::codepoint
        * @see inputChar(unsigned int codepoint)
        * @see inputString(const std::string &text)
        */
        void inputChar(const Line::codepoint &c);

        /**
        * Input a string to the terminal, typically due to a
        * user pasting text.
        * @note Not buffered
        * @param [in] text UTF-8 encoded string
        * @see inputChar(unsigned int codepoint)
        * @see inputChar(const Line::codepoint &c)
        */
        void inputString(const std::string &text);

        /**
        * Inputs an "action" key, one that is not printable but
        * represents some sort of command.
        * @param [in] key The pressed key
        */
        void inputActionKey(actionKey key);

        /**
        * Notify the terminal of user scroll input.
        * @param [in] yOffset The difference from the last scroll position
        * @param [in] mouseX Current x-coordinate of the mouse
        * @param [in] mouseY Current y-coordinate of the mouse
        */
        void inputMouseScroll(float yOffset, float mouseX, float mouseY);

        /**
        * Notify the terminal of user mouse button input.
        * For simplicity, the terminal doesn't care what mouse button was
        * pressed - however, it would probally be more intuitive if it was
        * the left mouse button.
        * @param [in] isPressed Was the mouse button pressed?
        * @param [in] mouseX Current x-coordinate of the mouse
        * @param [in] mouseY Current y-coordinate of the mouse
        */
        void inputMouseClick(bool isPressed, float mouseX, float mouseY);

        /**
        * Notify the terminal of user mouse movement.
        * @param [in] mouseX New x-coordinate of the mouse
        * @param [in] mouseY New y-coordinate of the mouse
        */
        void inputMouseMove(float mouseX, float mouseY);

        /**
        * Renders the terminal to the current framebuffer.
        * @warning The context within which the terminal was
        * created must be the one that's active due to the way
        * OpenGL resources are tied to a particular context.
        */
        void render();

        // Gets/sets if the terminal is focused by the user,
        // as determined by the terminal (mouse clicks).
        // Useful for determining where
        // a copy/paste is targeted
        /**
        * Gets if the terminal is focused by the user.
        * 
        * The default value is `true`.
        * 
        * This is determined by the user clicking inside the terminal
        * (`true`) or elseware (`false`).
        *
        * @note Used soley by you (the user) to determine if the terminal
        * should recieve input.
        * @see setFocused(bool val)
        * @return `true` if the terminal is focused.
        */
        bool isFocused();

        /**
        * Sets whether the terminal is focused or not.
        * 
        * The default value is `true`.
        * 
        * While normally determined by user mouse clicks,
        * if you feel the need you can manually set it yourself.
        *
        * @note Used soley by you (the user) to determine if the terminal
        * should recieve input. If your code does not rely on the value
        * of @ref isFocused(), a call to this does nothing.
        * @param [in] val New focused status
        */
        void setFocused(bool val);

        /**
        * Gets the text highlighted by the user.
        * 
        * This is useful for copy/paste operations - which
        * is exectly what this fuction was made for.
        * @return UTF-8 encoded string
        */
        std::string getTextSelection();
    };
}

#endif
