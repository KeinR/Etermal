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
#include "render/Framebuffer.h"

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
        /// Type used for @ref inputRequests
        typedef std::deque<TermInput*> inputRequests_t;

        /// The error callback.
        /// Recieves all erorrs; the terminal
        /// does nothing on its own when errors
        /// are detected (aside from calling this).
        /// Is always callable.
        errCallback_t errorCallback;

        /// The resources manager
        std::unique_ptr<Resources> resources;

        /// The viewport, describes the entire area taken
        /// up by the terminal.
        /// @see setWidth(int width)
        /// @see setHeight(int height)
        Model viewport;

        /// Scrollbar backend, keeps track of scroll
        Scroll scroll;
        /// Scrollbar frontend, shows scrollbar
        Scrollbar scrollbar;
        /// Mouse scroll factor
        float scrollSensitivity;

        /// The text processor
        TextBuffer display;
        /// The terminal background
        Rectangle background;

        /// UTF-8 encoded string waiting to
        /// be pushed to the @ref display
        std::string displayBuffer;

        /// All pending input requests
        inputRequests_t inputRequests;

        /// Is the terminal focused?
        /// Not actually used anywhere by the terminal -
        /// soley used by the client to determine if the
        /// terminal should recieve input.
        bool focused;
        /// Does the shell want user input?
        /// If false and @ref inputRequests is empty,
        /// ignores all user input.
        bool takeInput;

        /// If the next character should be escaped
        bool escapeNext;

        /// Cursor blink animation timer
        Timer cursorBlink;

        /// The tied shell
        EShell *shell;

        /// Called when the Terminal wants to set the
        /// cursor to its default state.
        winActionCB_t windowSetCursorDefault;
        /// Called when the Terminal wants to set the
        /// cursor to its I-Beam state.
        winActionCB_t windowSetCursorIBeam;
        /// Is the mouse hovering over the terminal input area?
        bool hovering;

        /// Is the mouse dragging?
        /// Used to determine what text is highlighted
        bool dragging;
        /// Mouse x at the last drag location
        float dragX;
        /// Mouse y at the last drag location
        float dragY;

        /// Framebuffer used to generate
        /// @ref framebufferTex
        Framebuffer framebuffer;
        /// Texture used to efficintly
        /// store and render the terminal.
        /// @see initTex()
        Texture framebufferTex;
        /// Whether @ref framebufferTex is valid.
        /// If it's not valid, will have to re-render
        /// all text again.
        /// @see invalidate()
        /// @see validate()
        bool framebufValid;

        /**
        * Invalidates the Terminal cache
        * [@ref framebufferTex], signaling
        * that it should be re-generated
        * upon the next call to @ref render().
        * @see validate()
        * @see framebufValid
        */
        void invalidate();
        /**
        * Validates the Terminal cache
        * [@ref framebufferTex], signaling
        * that it should @e NOT be re-generated
        * upon the next call to @ref render(),
        * avoiding squandering of resources
        * @see invalidate()
        * @see framebufValid
        */
        void validate();
        /**
        * Initialize the cache texture with the
        * dimensions of the terminal viewport.
        * @see framebufferTex
        * @see viewport
        */
        void initTex();

        /**
        * Display the welcome text.
        */
        void displayWelcome();

        /**
        * Tests if a codepoint is one that should be rejected
        * from the terminal input.
        * @param [in] c The codepoint in question
        * @return `true` if the codepoing should be rejected.
        */
        bool ignoreCodepoint(const Line::codepoint &c);

        // Pushes input to recievers (shell, etc)
        /**
        * Pushes input to recievers (shell, and anyone
        * listed in @ref inputRequests)
        * @param [in] input The input to push
        */
        void pushInput(const std::string &input);
        /**
        * Prepare the terminal for input.
        * Configures the cursor.
        */
        void prepareInput();
        /**
        * Input a codepoint to the @ref display
        */
        void doInputChar(const Line::codepoint &c);
        /**
        * SHould the terminal recieve user input?
        * Depends on if anyone wants input (shell, @ref inputRequests)
        * @return `true` if the terminal can recieve input
        */
        bool acceptInput();
        /**
        * Reads a 0-24 bit hex value from a string, starting right
        * after the provided index.
        * Returns with the index pointing to the last character in the hex.
        * Will stop prematurely if the hex cannot logically continue (chars
        * that are not in the range 0-9 or a-f, hit end of string)
        * @param [in] str The source string
        * @param [in,out] i The index, pointing to the char right before the hex starts
        * @return The hex value
        */
        int readHexFromStr(const std::string &str, std::string::size_type &i);
        /**
        * Maps screen coordinates to coordinates in the @ref display.
        * @param [in] x X screen coordinate
        * @param [in] y Y screen coordinate
        * @param [out] row Display row
        * @param [out] column Display column
        */
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
        * @param [in] value The value
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

        /**
        * Sets the pixel width of the terminal.
        * This does not affect the scrollbar width or
        * text width.
        * @param [in] width The new width
        * @see setColumns(int columns)
        * @see setHeight(int height)
        */
        void setWidth(int width);
        /**
        * Sets the pixel height of the terminal.
        * This does not affect the scrollbar height or
        * text height.
        * @param [in] height The new height
        * @see setRows(int rows, int margin)
        * @see setWidth(int width)
        */
        void setHeight(int height);
        /**
        * Sets the pixel height of the terminal
        * based off of a desired number of rows,
        * with one row being the height of a line of text.
        * This does not affect the scrollbar height or
        * text height.
        * @param [in] rows The desired rows
        * @param [in] margin Ammount of space at the bottom of the terminal,
        * so that the input area doesn't feel so cramped.
        * @see setHeight(int height)
        * @see setColumns(int columns)
        */
        void setRows(int rows, int margin = 7);
        /**
        * Sets the pixel width of the terminal
        * based off of a desired number of columns,
        * with one columns being the height of a single character.
        * This does not affect the scrollbar width or
        * text width.
        * @param [in] columns The desired columns
        * @see setWidth(int width)
        * @see setRows(int rows, int margin)
        */
        void setColumns(int columns);

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
        * @param [in] c The codepoint
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
        * @note Preserves the OpenGL state from when it was called,
        * so you don't have to worry about preserving your own state.
        * @see State
        */
        void render();

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
        * @return `true` if the terminal is focused.
        * @see setFocused(bool val)
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
        * @see isFocused()
        */
        void setFocused(bool val);

        /**
        * Gets the text highlighted by the user.
        * Highlight is determined by where the user clicking
        * and dragging.
        * 
        * This is useful for copy/paste operations - which
        * is exectly what this fuction was made for.
        * @return UTF-8 encoded string
        */
        std::string getTextSelection();
    };
}

#endif
