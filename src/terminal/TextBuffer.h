#ifndef ETERMAL_TEXTBUFFER_H_INCLUDED
#define ETERMAL_TEXTBUFFER_H_INCLUDED

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "gui/Rectangle.h"
#include "render/Color.h"
#include "Line.h"
#include "env.h"

#include "textmods/Mod.h"

namespace etm {
    // Resources
    class Resources;
    // render/Font
    class Font;
    // Scroll
    class Scroll;
    namespace tm {
        // textmods/TextState
        class TextState;
    }
}

namespace etm {

    /**
    * The core text processor and renderer.
    * @see Terminal
    * @see Line
    */
    class TextBuffer {
    public:
        /// Line type
        typedef Line line_t;
        /// Line index type
        typedef line_t::size_type line_index_t;
        /// List of lines
        typedef std::vector<line_t> lines_t;
        /// Index in line list
        typedef lines_t::size_type lines_number_t;

        /// modifier type
        typedef std::shared_ptr<tm::Mod> mod_t;
        /// modifier containter type
        typedef std::map<env::type, mod_t> modifierBlocks_t;

        /**
        * A position in the buffer
        */
        struct pos {
            /// The row/y
            lines_number_t row;
            /// The column/x
            line_index_t column;
            /**
            * Initializes all fields to zero.
            */
            pos();
            /**
            * Initializes with values.
            * @param [in] row The row/y
            * @param [in] column The column/x
            */
            pos(lines_number_t row, line_index_t column);
        };

    private:

        /// Handle to the resource manager
        Resources *res;
        /// Tells what parts of the buffer should be rendered.
        /// A very good optimization.
        Scroll *scroll;

        /// The max number of lines allowed.
        /// If this limit is exceeded, will start
        /// deleting the first lines.
        lines_number_t maxNumberLines;

        /// List of all lines
        lines_t lines;
        /// Max number if columns (@e not pixel width)
        line_index_t width;

        /// The position of the cursor
        pos cursor;
        /// The cursor minumum position
        pos cursorMin;
        // The cursor's display
        Rectangle dispCursor;

        /// Selection range start, for copying text.
        pos selectStart;
        /// Selection range end, for copying text.
        pos selectEnd;
        /// The @e actual selection start (smallest)
        /// @see selectStart
        pos *dfSelectStart;
        /// The @e actual selection end (largest)
        /// @see selectEnd
        pos *dfSelectEnd;

        /// Cursor's enabled state is manually controlled.
        /// The display state is intended to be controlled through
        /// animations (blinking), hence the toggleCursor() func.
        /// Whether the cursor is enabled.
        /// If `false` cursor will not be rendered.
        /// Same as @ref displayCursor but set in a different context.
        bool cursorEnabled;
        /// Whether the cursor is visible.
        /// If `false` cursor will not be rendered.
        /// Set during animations.
        bool displayCursor;

        /// Default text foreground color
        /// @see setDefForeGColor(const Color &color)
        Color defForegroundColor;
        /// Default text background color
        /// @see setDefBackGColor(const Color &color)
        Color defBackgroundColor;

        /// All the @ref TextState modifier blocks
        modifierBlocks_t modifierBlocks;
        /// The current modifier block id
        env::type blockId;

        // Checks if the number of lines is valid,
        // and truncates the lines if not.
        /**
        * Checks if the line count is less than or equal to
        * @ref maxNumberLines.
        * If it's not, deletes lines at the start until it's true.
        */
        void checkNumberLines();
        /**
        * Constructs a new line
        */
        void newline();
        // Inserts a newline after the given row
        /**
        * Inserts a newline after the given row.
        * @note Does not do bounds checks
        * @param [in] row Row after which to insert a line
        */
        void insertNewline(line_index_t row);
        /**
        * Check if the cursor is at the last column of
        * the last line.
        * @return `true` if yes
        */
        bool cursorAtEnd();
        /**
        * Append the given codepoint to the end of the last line.
        * If the last line's @e deFacto width would be larger than
        * @ref width, wraps the text.
        */
        void doAppend(const Line::codepoint &c);
        /**
        * Recalculates the text formatting by deleting all lines after
        * `row` and codepoints after and including `column` and appending
        * each one back on via @ref doAppend(const Line::codepoint &c).
        * @note Obviously, this is a little heavy, and we should look into
        * a better way of doing this that won't require writing a thousand
        * lines of code to account for every little case.
        * @param [in] row Row at which to start the reformatting
        * @param [in] column Column at which to start the reformatting (inclusive)
        */
        void reformat(lines_number_t row, line_index_t column);
        /**
        * Check if the given coordinates are out of range.
        * @return `true` if yes
        */
        bool outOfBounds(lines_number_t row, line_index_t column);
        /**
        * Erase a codepoint.
        * @note Does not do bounds checks
        * @param [in] row Character row
        * @param [in] column Character column
        */
        void doErase(lines_number_t row, line_index_t column);
        /**
        * Erase the last char of the last line.
        * @note Does not do bounds checks
        */
        void doTrunc();
        /**
        * Insert a codepoint at a position.
        * @note Does not do bounds checks
        * @param [in] row Row to insert into
        * @param [in] column Column in row to insert to
        * @param [in] c The codepoint to insert
        */
        void doInsert(lines_number_t row, line_index_t column, const Line::codepoint &c);

        /**
        * Deletes the first line.
        * @note Assumes that `lines.size() > 0`
        */
        void deleteFirstLine();
        /**
        * Deletes the last line.
        * @note Assumes that `lines.size() > 0`
        */
        void deleteLastLine();

        // Returns true if a line would qualify for a start space
        /**
        * Checks if a line would qualify for a "start space", a zero-width
        * space at the beginning.
        * @param [in] c The starting codepoint
        * @param [in] row The row/index of the line in question
        * @return `true` if yes
        */
        bool isStartSpace(const Line::codepoint &c, lines_number_t row);

        /**
        * Gets the font character width
        * @return The width
        */
        int charWidth();
        /**
        * Gets the font character height
        * @return The height
        */
        int charHeight();

        /**
        * Read a modifier index from the given line, starting at
        * the index, and return the corresponding modifier.
        * @param [in] ctrlIndex Index of the start of the control group, aka CONTROL_CHAR_START
        * @param [in] line The subject line
        */
        mod_t &getMod(Line::size_type ctrlIndex, Line &line);

        /**
        * Clamp the given row and column and store the values in p.
        * @param [out] p The output @ref pos
        * @param [in] row The row to clamp and store in `p`
        * @param [in] column The column to clamn and store in `p`
        */
        void clampPos(pos &p, lines_number_t row, line_index_t column);

        /**
        * Gets text in a given range.
        * @note Does range checks
        * @param [in] The start position
        * @param [in] The end position (column exclusive)
        * @return The text
        */
        std::string getTextFromRange(const pos &start, const pos &end);

    public:
        /**
        * Create a new buffer with arguments.
        * @param [in] res The resource manager
        * @param [in] scroll The scrollbar backend
        * @param [in] width Initial number of columns
        */
        TextBuffer(Resources *res, Scroll &scroll, line_index_t width);

        /**
        * Append a new modifier.
        * @param [in] mod The modifier
        */
        void pushMod(const std::shared_ptr<tm::Mod> &mod);

        /**
        * Delete all lines, and clear all caches.
        */
        void clear();

        /**
        * Set the max number of lines that will be kept
        * in memory.
        * @param [in] count The number of lines
        */
        void setMaxLines(lines_number_t count);

        /**
        * Set the default text foreground color.
        * @param [in] color The color
        * @see setDefBackGColor(const Color &color)
        */
        void setDefForeGColor(const Color &color);
        /**
        * Set the default text background color.
        * @param [in] color The color
        * @see setDefForeGColor(const Color &color)
        */
        void setDefBackGColor(const Color &color);

        /**
        * Gets the total height of all the rows combined.
        * @return The height
        */
        int getHeight();

        /**
        * Gets the number of rows
        * @return The number
        */
        lines_number_t getCountRows();

        /**
        * Gets the cursor's row
        * @return The row
        */
        lines_number_t getCursorRow();
        /**
        * Gets the cursor's column
        * @return The column
        */
        line_index_t getCursorCollumn();

        /**
        * Sets the cursor's minimum row
        * @param [in] row The row
        * @see setCursorMinCollumn(line_index_t column)
        */
        void setCursorMinRow(lines_number_t row);
        /**
        * Sets the cursor's minimum column
        * @param [in] column The column
        * @see setCursorMinRow(lines_number_t row)
        */
        void setCursorMinCollumn(line_index_t column);

        /**
        * Set the minimum cursor position to what it is currently.
        * @see setCursorMinRow(lines_number_t row)
        * @see setCursorMinCollumn(line_index_t column)
        */
        void lockCursor();

        /**
        * Move the cursor along its row, horizontally.
        * Will not move past line bounds.
        * @param [in] distance Distance to move. Negative values to move left, and vice versa.
        * @see moveCursorCollumnWrap(int distance)
        * @see moveCursorRow(int distance)
        */
        void moveCursorCollumn(int distance);
        /**
        * Move the cursor along its column, vertically.
        * Will not move past line count bounds.
        * Not garunteed to be in the same column due to differing line lengths.
        * @param [in] distance Distance to move. Positive values to move down, and vice versa.
        * @see moveCursorCollumn(int distance)
        */
        void moveCursorRow(int distance);

        /**
        * Like @ref moveCursorCollumn, except that if it encounters the
        * end of a line, it will jump to the start of the next line
        * (provided such a line exists), and vice versa.
        * @see moveCursorCollumn(int distance)
        * @see moveCursorRow(int distance)
        */
        void moveCursorCollumnWrap(int distance);

        /**
        * Move cursor to the last column of the last line/row,
        * aka the end.
        * @see moveCursorCollumn(int distance)
        * @see moveCursorRow(int distance)
        * @see cursorAtEnd()
        */
        void jumpCursor();
        /**
        * Set if the cursor should be enabled.
        * This is the first of the two values that
        * determine if the cursor should be displayed;
        * the second is at @ref toggleCursor().
        * However, since toggleCursor is intended to be
        * used for animations, we used a second "enabled"
        * value.
        * @param [in] val `true` to display the cursor
        * @see cursorIsEnabled()
        * @see toggleCursor()
        */
        void setCursorEnabled(bool val);
        /**
        * Check if the cursor is enabled.
        * @return `true` if yes
        */
        bool cursorIsEnabled();
        /**
        * Change the toggle value of the cursor to the opposate
        * of what it was.
        * AKA @e blink @e blink.
        */
        void toggleCursor();

        /**
        * Sets the color of the cursor.
        * @param [in] color The color
        */
        void setCursorColor(const Color &color);
        /**
        * The pixel width of the cursor.
        * For the average user, this is probally best at the default of `1`
        * @param [in] width The width of the cursor
        */
        void setCursorWidth(int width);

        /**
        * Sets the max number of columns for every line.
        * @note This triggers a reformatting of every single line,
        * so be careful about when you call this.
        * @param [in] width The number of columns
        * @see getWidth()
        * @see reformat()
        */
        void setWidth(line_index_t width);
        /**
        * Gets the max number of columns per line.
        * @return The number of columns
        * @see setWidth(line_index_t width)
        */
        line_index_t getWidth();

        /**
        * Erases the codepoint before the cursor, and deincrements the
        * cursor's index.
        * If the column is zero, will erase the last char on the next line.
        * If that'd be out of bounds, does nothing.
        * @see erase(lines_number_t row, line_index_t column)
        */
        void eraseAtCursor();
        /**
        * Erases a codepoint.
        * If out of bounds, does nothing.
        * @param [in] row The row of the codepoint
        * @param [in] column The column of the codepoint
        */
        void erase(lines_number_t row, line_index_t column);
        /**
        * Adds a codepoint to the end of the buffer.
        * @param [in] c The codepoint
        */
        void append(Line::codepoint c);
        /**
        * Removes the last codepoint.
        */
        void trunc();

        // Insert the char where the cursor is,
        // and moves it forward by one
        /**
        * Insert a codepoint where the cursor is,
        * and moves the cursor forward by one.
        * @param [in] c The codepoint
        * @see insert(lines_number_t row, line_index_t column, Line::codepoint c)
        */
        void insertAtCursor(Line::codepoint c);
        /**
        * Insert a codepoint into the buffer.
        * Does nothing if out of bounds.
        * @param [in] row The row
        * @param [in] column The column
        * @param [in] c The codepoint to insert
        */
        void insert(lines_number_t row, line_index_t column, Line::codepoint c);

        /**
        * Initialize the start and end-point of the selection zone.
        * @param [in] row The row
        * @param [in] column The column
        * @see setSelectionEnd(lines_number_t row, line_index_t column)
        * @see getSelectionText()
        */
        void initSelection(lines_number_t row, line_index_t column);
        /**
        * Change the location of the endpoint.
        * Can be any value, although you'd take care to @e min
        * your values as the parameters are unsigned, causing negative
        * values to be misrepresented.
        * @param [in] row The row
        * @param [in] column The column
        * @see initSelection(lines_number_t row, line_index_t column)
        * @see getSelectionText()
        */
        void setSelectionEnd(lines_number_t row, line_index_t column);
        /**
        * Gets the text highlighted by the selection / encapsulated
        * by the start/end of the selections.
        * Useful when copying text.
        * @return The selection text
        * @see initSelection(lines_number_t row, line_index_t column)
        * @see setSelectionEnd(lines_number_t row, line_index_t column)
        */
        std::string getSelectionText();
        /**
        * Gets the text after the min cursor point, aka the
        * text from the input area.
        * @return The input text
        * @see lockCursor()
        * @see getSelectionText()
        */
        std::string pollInput();

        /**
        * Prepares the buffer for textual appendations via @ref append().
        * append() does not call this method, so as to allow for efficient
        * chaining.
        */
        void prepare();

        /**
        * Renders the lines contined within the buffer to the
        * current framebuffer.
        * The given paramters determine the coordinate of the
        * top-left of the render area.
        * The height and width are determined by @ref width (columns)
        * and the value of @ref scroll's getNetWidth().
        * @note Requires that the text shader be set.
        * @note It is uncertain what shader will exit out with active.
        * If the cursor is rendering, it will be the primitive shader.
        * If not, it will be the text shader.
        * @note Only renders text as high as the value of @ref scroll's
        * getNetWidth() return. However, still has to do a lookbehind to
        * determine the current style, which can range from O(1) to O(n)
        * in complexity.
        * @param [in] x The x offset (terminal viewport x)
        * @param [in] y The y offset (terminal viewport y)
        */
        void render(int x, int y);
    };
}

#endif
