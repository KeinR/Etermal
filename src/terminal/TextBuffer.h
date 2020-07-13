#ifndef ETERMAL_TEXTBUFFER_H_INCLUDED
#define ETERMAL_TEXTBUFFER_H_INCLUDED

#include <vector>
#include <string>

#include "render/Model.h"

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
        Font *font;
        std::vector<bool> newlineChars;
        lines_t lines;
        line_index_t width;
        // Height is dynamic

        lines_number_t cursorRow;
        line_index_t cursorCollumn;

        lines_number_t cursorMinRow;
        line_index_t cursorMinCollumn;

        bool displayCursor;

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

    public:
        // Create text buffer with width
        TextBuffer(Font &font, line_index_t width);

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
        // Toggle display of the cursor
        void toggleCursor(bool val);
        // Change the toggle value of the cursor to the opposate
        // of what it was
        void switchCursorToggle();
        // Get the toggle state of the cursor
        bool cursorIsToggled();

        // Set the width.
        // Note that this is not reccomended as it'll
        // requre insertions (of whitespace) or deletions
        // (of glyphs) at multible locations to adjust the internal
        // buffer
        // void setWidth(line_index_t width);
        line_index_t getWidth();

        // (Rows = y, Collumns = x)
        // Rows inverted.
        // Row 0 would be the current row, 1 would the the
        // one above it.
        // Rows and collum values are automatically min/maxed
        // to prevent buffer over/underflows
        // void write(lines_number_t row, line_index_t collumn, char c);
        // // Unlike write(...), erase(...) will do nothing for indices that are
        // // out of bounds
        // void erase(lines_number_t row, line_index_t collumn);
        // Adds the char to the end of the buffer
        void append(char c);

        // Insert the char where the cursor is,
        // and moves it forward by one
        void insertAtCursor(char c);
        // Called by insertAtCursor
        void insert(lines_number_t row, line_index_t collumn, char c);

        void render(Resources *res);
    };
}

#endif
