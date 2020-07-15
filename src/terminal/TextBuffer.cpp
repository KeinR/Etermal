#include "TextBuffer.h"

#include <iostream>
#include <cmath>

#include "gui/Rectangle.h"
#include "Resources.h"
#include "util/util.h" // TEMP

etm::TextBuffer::pos::pos(): pos(0, 0) {
}
etm::TextBuffer::pos::pos(lines_number_t row, line_index_t column):
    row(row), column(column) {
}

etm::TextBuffer::TextBuffer(Resources *res, line_index_t width):
    res(res), width(width), dispCursor(res),
    cursorEnabled(false), displayCursor(false)
{
    setDefForeGColor(0xffffff);
    setDefBackGColor(0x000000);
    setCursorColor(0xffffff);
    setCursorWidth(1);
}

void etm::TextBuffer::newline() {
    lines.emplace_back();
    lines.back().reserve(width);
}

void etm::TextBuffer::insertNewline(line_index_t row) {
    row++;
    lines.insert(lines.begin() + row, line_t());
    lines[row].reserve(width);
}

bool etm::TextBuffer::cursorAtEnd() {
    return cursor.row == lines.size() - 1 && cursor.column == lines[cursor.row].size();
}

bool etm::TextBuffer::outOfBounds(lines_number_t row, line_index_t column) {
    return row >= lines.size() || column >= lines[row].size();
}

int etm::TextBuffer::charWidth() {
    return res->getFont().getCharWidth();
}
int etm::TextBuffer::charHeight() {
    return res->getFont().getCharHeight();
}

void etm::TextBuffer::setDefForeGColor(const Color &color) {
    defForegroundColor = color;
}
void etm::TextBuffer::setDefBackGColor(const Color &color) {
    defBackgroundColor = color;
}

int etm::TextBuffer::getHeight() {
    return static_cast<int>(lines.size()) * charHeight();
}

etm::TextBuffer::lines_number_t etm::TextBuffer::getCountRows() {
    return lines.size();
}

etm::TextBuffer::lines_number_t etm::TextBuffer::getCursorRow() {
    return cursor.row;
}
etm::TextBuffer::line_index_t etm::TextBuffer::getCursorCollumn() {
    return cursor.column;
}

void etm::TextBuffer::setCursorMinRow(lines_number_t row) {
    cursorMin.row = row;
}
void etm::TextBuffer::setCursorMinCollumn(line_index_t column) {
    cursorMin.column = column;
}

void etm::TextBuffer::lockCursor() {
    setCursorMinRow(cursor.row);
    setCursorMinCollumn(cursor.column);
}


void etm::TextBuffer::moveCursorCollumn(int distance) {
    cursor.column = std::min(cursor.column + distance, lines[cursor.row].size());
    if (cursor.row <= cursorMin.row) {
        cursor.column = std::max(cursor.column, cursorMin.column);
    }
}
void etm::TextBuffer::moveCursorRow(int distance) {
    cursor.row = std::min(std::max(cursor.row + distance, cursorMin.row), lines.size()-1);
    cursor.column = std::min(cursor.column, lines[cursor.row].size());
}
void etm::TextBuffer::moveCursorCollumnWrap(int distance) {
    int columnTemp = static_cast<int>(cursor.column) + distance;

    if (columnTemp > static_cast<int>(lines[cursor.row].size())) {
        cursor.column = static_cast<line_index_t>(columnTemp);
        while (cursor.column > lines[cursor.row].size()) {
            if (cursor.row + 1 < lines.size()) {
                cursor.column -= lines[cursor.row].size();
                cursor.row++;
            } else {
                cursor.column = lines[cursor.row].size();
                break;
            }
        }
    } else if (columnTemp < 0) {
        // columnTemp += static_cast<int>(oldCursorCollumn);
        while (columnTemp < 0) {
            if (cursor.row - 1 >= cursorMin.row) {
                cursor.row--;
                columnTemp += lines[cursor.row].size();
            } else {
                columnTemp = 0;
                break;
            }
        }
        cursor.column = static_cast<line_index_t>(columnTemp);
    } else {
        cursor.column = static_cast<line_index_t>(std::max(columnTemp, 0));
        // Wrap cursor to next line.
        // I mean MinTTY does it so really there's no reason not to
        if (cursor.row < lines.size() - 1 && cursor.column == lines[cursor.row].size()) {
            cursor.row++;
            cursor.column = 0;
        }
    }
    if (cursor.row <= cursorMin.row) {
        cursor.column = std::max(cursor.column, cursorMin.column);
    }
}

void etm::TextBuffer::jumpCursor() {
    cursor.row = lines.size() - 1;
    cursor.column = lines[cursor.row].size();
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

void etm::TextBuffer::setCursorColor(const Color &color) {
    dispCursor.setColor(color);
}

void etm::TextBuffer::setCursorWidth(int value) {
    dispCursor.setWidth(value);
}


void etm::TextBuffer::setWidth(line_index_t width) {
    this->width = width;
}
etm::TextBuffer::line_index_t etm::TextBuffer::getWidth() {
    return width;
}

void etm::TextBuffer::doAppend(const Character &c) {
    if (c.getValue() == '\n') {
        lines.back().setNewline(true);
        newline();
    } else if (!lines.size()) {
        newline();
        lines.back().append(c);
    } else if (lines.back().size() + 1 > width) {
        newline(); // Warning: refs invalidated after call!
        line_t &lastLine = lines[lines.size() - 2];
        line_t &nextLine = lines[lines.size() - 1];

        // If the last char or this char is NOT a space, do word wrap
                                    // because it's unsigned;
                                    // check for less than zero
        if (c.getValue() != ' ' && (lastLine.size() - 1 > lastLine.size() || lastLine[lastLine.size()-1].getValue() != ' ')) {
            // -2 because we already checked that -1 wasn't a space
            // Check less than while deincrementing because unsigned
            for (line_index_t i = lastLine.size() - 2; i < lastLine.size(); i--) {
                if (lastLine[i].getValue() == ' ') {
                    const line_index_t index = i;
                    i++;
                    // Move the last word from the last line to the next
                    // line via recursion
                    for (; i < lastLine.size(); i++) {
                        append(lastLine[i]);
                    }

                    lastLine.erase(index+1);
                    std::cout << "index = " << index << std::endl;
                    std::cout << "last line size " << lastLine.size() << std::endl;
                    break;
                }
            }
            append(c);
        } else {
            // Prevent stack overflow from width of 0...
            // should never happen, but I like to have a reason
            // to avoid recursion
            nextLine.append(c);
        }
    } else {
        lines.back().append(c);
    }
}

void etm::TextBuffer::write(lines_number_t row, line_index_t column, const Character &c) {
    if (outOfBounds(row, column)) return;

    if (column == lines[row].size()-1 && lines[row].hasNewline() && c.getValue() != '\n') {
        // If was the last char and the line was broken manually,
        // instead it deleted the newline
        lines[row].setNewline(false);
        lines[row].append(c);
        reformat(row, column);
    } else {
        lines[row][column] = c;
    }
}

void etm::TextBuffer::eraseAtCursor() {
    lines_number_t row = cursor.row;
    line_index_t column = cursor.column - 1;
    // Because it's unsigned it underflows to big number
    if (column > lines[row].size()) {
        row--;
        // Because it's unsigned it underflows to big number
        if (row < lines.size() && row >= cursorMin.row) {
            column = lines[row].size();
        } else {
            return;
        }
    }
    if (column >= cursorMin.column && column < lines[row].size()) {
        if (cursorAtEnd()) {
            doTrunc();
            jumpCursor();
        } else {
            erase(row, column);
            cursor.column = column;
            cursor.row = row;
        }
    }
}

void etm::TextBuffer::erase(lines_number_t row, line_index_t column) {
    if (!outOfBounds(row, column)) {
        doErase(row, column);
    }
}
void etm::TextBuffer::doErase(lines_number_t row, line_index_t column) {
    // TODO: reformating needs to be optimized

    if (column == lines[row].size()-1 && lines[row].hasNewline()) {
        // If was the last char and the line was broken manually,
        // instead it deleted the newline
        lines[row].setNewline(false);
        reformat(row, column);
    } else {
        lines[row].erase(column, 1);
        reformat(row, column-1);
    }

}

void etm::TextBuffer::append(const Character &c) {
    const bool moveCursor = cursorAtEnd();
    doAppend(c);
    if (moveCursor) {
        jumpCursor();
    }
}

void etm::TextBuffer::trunc() {
    const bool moveCursor = cursorAtEnd();
    doTrunc();
    if (moveCursor) {
        jumpCursor();
    }
}

void etm::TextBuffer::doTrunc() {

    if (lines.size()) {
        if (!lines.back().size()) {
            // If line empty, delete it
            if (lines.size() > 1) {
                // Only if this isn't the last line
                lines.pop_back();
            } else {
                // This is the last line and it has no chars.
                // Deny everything.
                return;
            }
        }
        lines.back().popBack();
        // Don't delete line if it's the only one.
        if (lines.size() > 1 && !lines[lines.size() - 2].hasNewline()) {
            // Delete line if empty and last line didn't force
            // a newline break.
            if (!lines.back().size()) {
                lines.pop_back();
            } else if (width - lines[lines.size()-2].size() >= lines.back().size()) {
                // Move data, as it can sort-of reverse-wrap back to the previous line
                lines[lines.size()-2].appendOther(lines.back());
                lines.pop_back();
            }
        }
    }

}

void etm::TextBuffer::insertAtCursor(const Character &c) {
    if (cursorAtEnd()) {
        doAppend(c);
    } else {
        insert(cursor.row, cursor.column, c);
    }
    moveCursorCollumnWrap(1);
}

void etm::TextBuffer::insert(lines_number_t row, line_index_t column, const Character &c) {
    if (outOfBounds(row, column)) return;

    if (c.getValue() == '\n') {
        if (!lines[row].hasNewline()) {            
            // Move the text after the newline to the newly created line
            Line::string_t buffer(lines[row].substr(column));
            lines[row].erase(column);
            lines[row].setNewline(true);
            if (row + 1 >= lines.size()) {
                newline();
                lines[row+1].appendStr(buffer);
            } else {
                lines[row+1].insertStr(0, buffer);
                if (lines[row+1].size() > width) {
                    reformat(row + 1, 0);
                }
            }
        } else {
            insertNewline(row);
        }
    } else {
        lines[row].insert(column, c);
        if (lines[row].size() > width) {
            reformat(row, column);
        }
    }

}

void etm::TextBuffer::reformat(lines_number_t row, line_index_t column) {
    Line::string_t buffer(lines[row].substr(column));
    lines[row].erase(column);
    for (lines_number_t r = row+1; r < lines.size(); r++) {
        lines[r].copyTo(buffer);
        if (lines[r].hasNewline()) {
            buffer.emplace_back('\n');
        }
    }
    lines.erase(lines.begin() + row + 1, lines.end());
    for (Character &c : buffer) {
        doAppend(c);
    }
}

void etm::TextBuffer::bindChar(char c) {
    textCache_t::iterator loc = textCache.find(c);
    if (loc != textCache.end()) {
        loc->second.bind();
    } else {
        textCache[c] = res->getFont().renderChar(c);
        textCache[c].bind();
    }
}

void etm::TextBuffer::render(int x, int y) {

    if (cursorEnabled && displayCursor) {
        // Assume that the primitive shader was already set by
        // the terminal
        dispCursor.setX(cursor.column * charWidth());
        dispCursor.setY(cursor.row * charHeight());
        dispCursor.setHeight(charHeight());
        dispCursor.render();
    }

    res->bindTextShader();

    Model model(x, y, charWidth(), charHeight());


    Color *backgroundColor = &defBackgroundColor;
    Color *foregroundColor = &defForegroundColor;

    assertGLErr("Pre-color sset");
    backgroundColor->setBackground(res->getShader());
    assertGLErr("mid-color sset");
    foregroundColor->setForeground(res->getShader());
    assertGLErr("post-color sset");

    for (lines_number_t r = 0; r < lines.size(); r++) {
        line_t &line = lines[r];
        for (line_index_t c = 0; c < line.size(); c++) {
            Character &chr = line[c];
            if (chr.hasBackGC() && chr.getBackGC() != *backgroundColor) {
                backgroundColor = &chr.getBackGC();
                backgroundColor->setBackground(res->getShader());
            }
            if (chr.hasForeGC() && chr.getForeGC() != *foregroundColor) {
                foregroundColor = &chr.getForeGC();
                foregroundColor->setForeground(res->getShader());
            }
            bindChar(chr.getValue());
            model.set(res->getShader());
            res->renderRectangle();
            model.x += charWidth();
        }
        model.x = 0;
        model.y += charHeight();
    }
}
