#include "TextBuffer.h"

#include <iostream>
#include <cmath>

#include "gui/Rectangle.h"
#include "gui/Image.h"

etm::TextBuffer::TextBuffer(Font &font, line_index_t width):
    font(&font), width(width),
    cursorRow(0), cursorCollumn(0),
    cursorMinRow(0), cursorMinCollumn(0),
    cursorEnabled(false), displayCursor(false) {
}

void etm::TextBuffer::newline() {
    lines.emplace_back();
    lines.back().reserve(width);
    newlineChars.emplace_back(false);
}

void etm::TextBuffer::insertNewline(line_index_t row) {
    row++;
    lines.insert(lines.begin() + row, line_t());
    lines[row].reserve(width);
    newlineChars.insert(newlineChars.begin() + row, false);
}

bool etm::TextBuffer::cursorAtEnd() {
    return cursorRow == lines.size() - 1 && cursorCollumn == lines[cursorRow].size();
}

bool etm::TextBuffer::outOfBounds(lines_number_t row, line_index_t collumn) {
    return row >= lines.size() || collumn >= lines[row].size();
}

etm::TextBuffer::lines_number_t etm::TextBuffer::getCountRows() {
    return lines.size();
}

etm::TextBuffer::lines_number_t etm::TextBuffer::getCursorRow() {
    return cursorRow;
}
etm::TextBuffer::line_index_t etm::TextBuffer::getCursorCollumn() {
    return cursorCollumn;
}

void etm::TextBuffer::setCursorMinRow(lines_number_t row) {
    cursorMinRow = row;
}
void etm::TextBuffer::setCursorMinCollumn(line_index_t collumn) {
    cursorMinCollumn = collumn;
}

void etm::TextBuffer::lockCursor() {
    setCursorMinRow(cursorRow);
    setCursorMinCollumn(cursorCollumn);
}


void etm::TextBuffer::moveCursorCollumn(int distance) {
    cursorCollumn = std::min(cursorCollumn + distance, lines[cursorRow].size());
    if (cursorRow <= cursorMinRow) {
        cursorCollumn = std::max(cursorCollumn, cursorMinCollumn);
    }
}
void etm::TextBuffer::moveCursorRow(int distance) {
    cursorRow = std::min(std::max(cursorRow + distance, cursorMinRow), lines.size()-1);
    cursorCollumn = std::min(cursorCollumn, lines[cursorRow].size());
}
void etm::TextBuffer::moveCursorCollumnWrap(int distance) {
    int collumnTemp = static_cast<int>(cursorCollumn) + distance;

    if (collumnTemp > static_cast<int>(lines[cursorRow].size())) {
        cursorCollumn = static_cast<line_index_t>(collumnTemp);
        while (cursorCollumn > lines[cursorRow].size()) {
            if (cursorRow + 1 < lines.size()) {
                cursorCollumn -= lines[cursorRow].size();
                cursorRow++;
            } else {
                cursorCollumn = lines[cursorRow].size();
                break;
            }
        }
    } else if (collumnTemp < 0) {
        // collumnTemp += static_cast<int>(oldCursorCollumn);
        while (collumnTemp < 0) {
            if (cursorRow - 1 >= cursorMinRow) {
                cursorRow--;
                collumnTemp += lines[cursorRow].size();
            } else {
                collumnTemp = 0;
                break;
            }
        }
        cursorCollumn = static_cast<line_index_t>(collumnTemp);
    } else {
        cursorCollumn = static_cast<line_index_t>(std::max(collumnTemp, 0));
        // Wrap cursor to next line.
        // I mean MinTTY does it so really there's no reason not to
        if (cursorRow < lines.size() - 1 && cursorCollumn == lines[cursorRow].size()) {
            cursorRow++;
            cursorCollumn = 0;
        }
    }
    if (cursorRow <= cursorMinRow) {
        cursorCollumn = std::max(cursorCollumn, cursorMinCollumn);
    }
}

void etm::TextBuffer::jumpCursor() {
    cursorRow = lines.size() - 1;
    cursorCollumn = lines[cursorRow].size();
}
void etm::TextBuffer::setCursorEnabled(bool val) {
    cursorEnabled = val;
}
bool etm::TextBuffer::cursorIsEnabled() {
    return cursorEnabled;
}
void etm::TextBuffer::toggleCursor() {
    displayCursor = !displayCursor;
}


void etm::TextBuffer::setWidth(line_index_t width) {
    this->width = width;
}
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

void etm::TextBuffer::doAppend(char c) {
    if (c == '\n') {
        newlineChars.back() = true;
        newline();
    } else if (!lines.size()) {
        newline();
        lines.back().push_back(c);
    } else if (lines.back().size() + 1 > width) {
        newline(); // Warning: refs invalidated after call!
        line_t &lastLine = lines[lines.size() - 2];
        line_t &nextLine = lines[lines.size() - 1];

        // If the last char or this char is NOT a space, do word wrap 
        if (c != ' ' && (lastLine.size() - 1 < 0 || lastLine[lastLine.size()-1] != ' ')) {
            // -2 because we already checked that -1 wasn't a space
            // Check less than while deincrementing because unsigned
            for (line_index_t i = lastLine.size() - 2; i < lastLine.size(); i--) {
                if (lastLine[i] == ' ') {
                    const line_index_t index = i;
                    i++;
                    // Move the last word from the last line to the next
                    // line via recursion
                    for (; i < lastLine.size(); i++) {
                        append(lastLine[i]);
                        lastLine[i] = ' '; // TEMP, in future want to add wrap flag
                    }
                    lastLine.erase(index+1);
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

void etm::TextBuffer::write(lines_number_t row, line_index_t collumn, char c) {
    if (outOfBounds(row, collumn)) return;
}
void etm::TextBuffer::erase(lines_number_t row, line_index_t collumn) {
    if (outOfBounds(row, collumn)) return;
    
}

void etm::TextBuffer::append(char c) {
    const bool moveCursor = cursorAtEnd();
    doAppend(c);
    if (moveCursor) {
        jumpCursor();
    }
}

void etm::TextBuffer::insertAtCursor(char c) {
    if (cursorAtEnd()) {
        doAppend(c);
    } else {
        insert(cursorRow, cursorCollumn, c);
    }
    moveCursorCollumnWrap(1);
}

void etm::TextBuffer::insert(lines_number_t row, line_index_t collumn, char c) {
    if (outOfBounds(row, collumn)) return;

    if (c == '\n') {
        // Move the text after the newline to the newly created line
        std::string buffer(lines[row].substr(collumn));
        lines[row].erase(collumn);
        insertNewline(row);
        lines[row+1] += buffer;
        newlineChars[row] = true;
        if (!newlineChars[row+1]) {
            reformat(row + 1, 0);
        }
    } else {
        lines[row].insert(lines[row].begin() + collumn, c);
        if (lines[row].size() > width) {
            reformat(row, collumn);
        }
    }

}

void etm::TextBuffer::reformat(lines_number_t row, line_index_t collumn) {
    std::string buffer;
    buffer += lines[row].substr(collumn);
    lines[row].erase(collumn);
    for (lines_number_t r = row+1; r < lines.size(); r++) {
        buffer += lines[r];
        if (newlineChars[r]) {
            buffer.push_back('\n');
        }
    }
    lines.erase(lines.begin() + row + 1, lines.end());
    newlineChars.erase(newlineChars.begin() + row + 1, newlineChars.end());
    for (char c : buffer) {
        doAppend(c);
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

    if (cursorEnabled && displayCursor) {
        // Assume that the primitive shader was already set by
        // the terminal
        Rectangle cursor(res);
        cursor.setColor(0xffffff);
        cursor.setX(cursorCollumn * advance);
        cursor.setY(cursorRow * lineHeight);
        cursor.setWidth(1);
        cursor.setHeight(lineHeight);
        cursor.render();
    }

    res->bindTextureShader();

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
