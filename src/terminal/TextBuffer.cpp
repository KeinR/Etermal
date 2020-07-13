#include "TextBuffer.h"

#include <iostream>

#include "gui/Image.h"

etm::TextBuffer::TextBuffer(Font &font, line_index_t width):
    font(&font), width(width) {
}

void etm::TextBuffer::newline() {
    lines.emplace_back();
    lines.back().reserve(width);
}

// void etm::TextBuffer::setWidth(data_index_t width) {

// }
etm::TextBuffer::line_index_t etm::TextBuffer::getWidth() {
    return width;
}

// void etm::TextBuffer::write(data_index_t row, data_index_t collumn, char c) {
//     // const int height = data.size() / width; // Zero index'd
//     // if (row > height) {
//     //     row = height;
//     // } else if (row < 0) {
//     //     row = 0;
//     // }
//     // if (collumn > width) {
//     //     collumn = width;
//     // } else if (collumn < 0) {
//     //     collumn = 0;
//     // }



// }

// void etm::TextBuffer::erase(data_index_t row, data_index_t collumn) {
// }

void etm::TextBuffer::append(char c) {
    if (c == '\n') {
        newline();
    } else if (!lines.size()) {
        newline();
        lines.back().push_back(c);
    } else if (lines.back().size() + 1 > width) {
        newline(); // Warning: refs invalidated after call!
        line_t &lastLine = lines[lines.size() - 1];
        line_t &nextLine = lines[lines.size() - 2];

        // If the last char or this char is NOT a space, do word wrap 
        if (c != ' ' && (lastLine.size() - 1 < 0 || lastLine[lastLine.size()-1] != ' ')) {
            // -2 because we already checked that -1 wasn't a space
            // Check less than while deincrementing because unsigned
            for (unsigned int i = lastLine.size() - 2; i < lastLine.size(); i--) {
                if (lastLine[i] == ' ') {
                    i++;
                    // Move the last word from the last line to the next
                    // line via recursion
                    for (; i < lastLine.size(); i++) {
                        append(lastLine[i]);
                        lastLine[i] = ' '; // TEMP, in future want to add wrap flag
                    }
                    break;
                }
            }
            append(c);
        } else {
            // Prevent stack overflow from width of 0...
            // should never happen, but I like to have a reason
            // to avoid recursion
            nextLine.push_back(c);
        }
    } else {
        lines.back().push_back(c);
    }
}

void etm::TextBuffer::render(Resources *res) {
    Image img(res);
    int x = 0;
    int y = 0;
    const int lineHeight = font->getFace()->size->metrics.height / 64;
    const int advance = font->getFace()->size->metrics.max_advance / 64;
    img.setWidth(advance);
    img.setHeight(lineHeight);
    for (lines_number_t r = 0; r < lines.size(); r++) {
        line_t &line = lines[r];
        for (line_index_t c = 0; c < line.size(); c++) {
            font->renderChar(x, y, line[c], img);
            img.setX(x);
            img.setY(y);
            img.render();
            x += advance;
        }
        x = 0;
        y += lineHeight;
    }
}
