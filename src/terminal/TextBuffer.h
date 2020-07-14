#ifndef ETERMAL_TEXTBUFFER_H_INCLUDED
#define ETERMAL_TEXTBUFFER_H_INCLUDED

#include <vector>
#include <string>
#include <map>

#include "render/Model.h"
#include "render/Texture.h"

#include "Resources.h" // TEMP

namespace etm { class Font; }

namespace etm {
    class TextBuffer {
    public:
        typedef std::string line_t;
        typedef line_t::size_type line_index_t;
        typedef std::vector<line_t> lines_t;
        typedef lines_t::size_type lines_number_t;
    private:
        typedef std::map<char, Texture> textCache_t;

        Resources *res;
        std::vector<bool> newlineChars;
        lines_t lines;
        line_index_t width;
        // Height is dynamic

        lines_number_t cursorRow;
        line_index_t cursorCollumn;

        lines_number_t cursorMinRow;
        line_index_t cursorMinCollumn;

        bool cursorEnabled;
        bool displayCursor;

        // Must be cleared upon font change
        textCache_t textCache;

        // Construct a new line
        void newline();
        // Inserts a newline after the given row
        void insertNewline(line_index_t row);
        bool cursorAtEnd();
        void doAppend(char c);
        // Re-appends all chars after and including row and collumn to re-format the text
        void reformat(lines_number_t row, line_index_t collumn);
        void checkCursorCollumn();
        void checkCursorRow();
        // Returns true if the coords are out of bounds
        bool outOfBounds(lines_number_t row, line_index_t collumn);
        void doErase(lines_number_t row, line_index_t collumn);
        void doTrunc();

        // Bind texture for char
        void bindChar(char c);

    public:
        // Create text buffer with width
        TextBuffer(Resources *res, line_index_t width);

        // The (visible) height of all the rows combined
        int getHeight();

        lines_number_t getCountRows();

        lines_number_t getCursorRow();
        line_index_t getCursorCollumn();

        void setCursorMinRow(lines_number_t row);
        void setCursorMinCollumn(line_index_t collumn);

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

        // Set the width.
        // Note that this is not reccomended as it'll
        // requre insertions (of whitespace) or deletions
        // (of glyphs) at multible locations to adjust the internal
        // buffer
        void setWidth(line_index_t width);
        line_index_t getWidth();

        // Overwrites character at row, collumn.
        // Does nothing if out of bounds.
        void write(lines_number_t row, line_index_t collumn, char c);
        // Erases the character before the cursor, and deincrements the
        // cursor's index.
        // If the collumn is zero, will erase the last char on the next line.
        // If that'd be out of bounds, does nothing.
        void eraseAtCursor();
        // Erases character at row, collumn.
        // If out of bounds, does nothing.
        void erase(lines_number_t row, line_index_t collumn);
        // Adds the char to the end of the buffer
        void append(char c);
        // Removes the last char
        void trunc();

        // Insert the char where the cursor is,
        // and moves it forward by one
        void insertAtCursor(char c);
        // Called by insertAtCursor
        void insert(lines_number_t row, line_index_t collumn, char c);

        // ---Assumes that the primitive shader has already been set---
        void render(int x, int y);
    };
}

#endif
