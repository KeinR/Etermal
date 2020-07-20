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
    class TextBuffer {
    public:
        typedef Line line_t;
        typedef line_t::size_type line_index_t;
        typedef std::vector<line_t> lines_t;
        typedef lines_t::size_type lines_number_t;

        typedef std::shared_ptr<tm::Mod> mod_t;
        typedef std::map<env::type, mod_t> modifierBlocks_t;

        struct pos {
            lines_number_t row;
            line_index_t column;
            // Zero init
            pos();
            pos(lines_number_t row, line_index_t column);
        };

    private:

        Resources *res;
        // Tells what parts of self should render.
        // A very useful optimization.
        Scroll *scroll;

        // The max number of lines allowed.
        // If this limit is exceeded, will start
        // deleting the first lines.
        lines_number_t maxNumberLines;

        lines_t lines;
        line_index_t width;
        // Height is dynamic

        pos cursor;
        pos cursorMin;
        Rectangle dispCursor; // Cursor display

        // Selection range, for copying text.
        // Characters in this range have inverted colors
        // (background/foreground swapped)
        pos selectStart;
        pos selectEnd;
        // Pointers to the _actual_ start and end
        pos *dfSelectStart;
        pos *dfSelectEnd;

        // Cursor's enabled state is manually controlled.
        // The display state is intended to be controlled through
        // animations (blinking), hence the toggleCursor() func
        bool cursorEnabled;
        bool displayCursor;

        // Default fore/back ground colors
        Color defForegroundColor;
        Color defBackgroundColor;

        modifierBlocks_t modifierBlocks;
        env::type blockId;

        // Checks if the number of lines is valid,
        // and truncates the lines if not.
        void checkNumberLines();
        // Construct a new line
        void newline();
        // Inserts a newline after the given row
        void insertNewline(line_index_t row);
        bool cursorAtEnd();
        void doAppend(const Line::codepoint &c);
        // Re-appends all chars after and including row and column to re-format the text
        void reformat(lines_number_t row, line_index_t column);
        void checkCursorCollumn();
        void checkCursorRow();
        // Returns true if the coords are out of bounds
        bool outOfBounds(lines_number_t row, line_index_t column);
        void doErase(lines_number_t row, line_index_t column);
        void doTrunc();
        void doInsert(lines_number_t row, line_index_t column, const Line::codepoint &c);

        // Deletes the first line.
        // Assumes that lines.size() > 0
        void deleteFirstLine();
        // Deletes the last line.
        // Assumes that lines.size() > 0
        void deleteLastLine();

        // Returns true if a line would qualify for a start space
        bool isStartSpace(const Line::codepoint &c, lines_number_t row);

        // Wrappers for calls to
        // res->getFont().getCharWidth() and
        // res->getFont().getCharHeight() respectively
        int charWidth();
        int charHeight();

        mod_t &getMod(Line::size_type ctrlIndex, Line &line);

        // Clamp the position given
        void clampPos(pos &p, lines_number_t row, line_index_t column);

        // Gets the text in range, start inclusive end->column exclusive
        std::string getTextFromRange(const pos &start, const pos &end);

    public:
        // Create text buffer with width
        TextBuffer(Resources *res, Scroll &scroll, line_index_t width);

        void pushMod(const std::shared_ptr<tm::Mod> &mod);

        void clear();

        // Set the max lines that will be kep in memory
        void setMaxLines(lines_number_t count);

        void setDefForeGColor(const Color &color);
        void setDefBackGColor(const Color &color);

        // The (visible) height of all the rows combined
        int getHeight();

        lines_number_t getCountRows();

        lines_number_t getCursorRow();
        line_index_t getCursorCollumn();

        void setCursorMinRow(lines_number_t row);
        void setCursorMinCollumn(line_index_t column);

        // Prevent cursor from moving before its location
        void lockCursor();

        // Positive distance up/right, negative for down/left.
        // If values are above/below max/min, moves max/min.
        void moveCursorCollumn(int distance);
        void moveCursorRow(int distance);

        // Like its namesake, however will move to the next/previous line
        // to deal with large distance values.
        // Still guards against values that'd take it past the last line
        // or before the first.
        void moveCursorCollumnWrap(int distance);

        // Move cursor to end of input
        void jumpCursor();
        // Set displaying of the cursor.
        // Different from the toggle state.
        void setCursorEnabled(bool val);
        bool cursorIsEnabled();
        // Change the toggle value of the cursor to the opposate
        // of what it was.
        // aka blink blink
        void toggleCursor();

        // Sets the color of the little blinky boi
        void setCursorColor(const Color &color);
        // The pixel width of the cursor.
        // For the average user, this is probally best at the default (1)
        void setCursorWidth(int width);

        // Set the width.
        // Note that this is not reccomended as it'll
        // requre insertions (of whitespace) or deletions
        // (of glyphs) at multible locations to adjust the internal
        // buffer
        void setWidth(line_index_t width);
        line_index_t getWidth();

        // Erases the character before the cursor, and deincrements the
        // cursor's index.
        // If the column is zero, will erase the last char on the next line.
        // If that'd be out of bounds, does nothing.
        void eraseAtCursor();
        // Erases character at row, column.
        // If out of bounds, does nothing.
        void erase(lines_number_t row, line_index_t column);
        // Adds the char to the end of the buffer.
        // Params describe the range for the codepoint, inclusive-exclusive
        void append(Line::codepoint c);
        // Removes the last char
        void trunc();

        // Insert the char where the cursor is,
        // and moves it forward by one
        void insertAtCursor(Line::codepoint c);
        // Called by insertAtCursor
        void insert(lines_number_t row, line_index_t column, Line::codepoint c);

        // Initialize the start and endpoint of the selection zone
        void initSelection(lines_number_t row, line_index_t column);
        // Change the location of the endpoint; can be any value,
        // although you'd take care to std::min your values as the parameters
        // are unsigned
        void setSelectionEnd(lines_number_t row, line_index_t column);
        // Gets the text highlighted by the selection;
        // used only really when copying text
        std::string getSelectionText();
        // Gets the text after the min cursor point, aka the
        // text from the input area
        std::string pollInput();

        // Prepares the buffer for textual appendations (append(...)).
        // append(...) does not call this method, because appends
        // can be chained with flushes.
        void prepare();

        // ---Assumes that the primitive shader has already been set---
        void render(int x, int y);
    };
}

#endif
