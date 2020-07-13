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
        lines_t lines;
        line_index_t width;
        // Height is dynamic

        lines_number_t cursorRow;
        line_index_t cursorCollumn;

        // Construct a new line
        void newline();

    public:
        // Create text buffer with width
        TextBuffer(Font &font, line_index_t width);

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

        void render(Resources *res);
    };
}

#endif
