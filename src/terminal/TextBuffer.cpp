#include "TextBuffer.h"

#include <iostream>
#include <cmath>

#include "gui/Rectangle.h"
#include "Resources.h"
#include "util/util.h" // TEMP
#include "Scroll.h"
#include "env.h"
#include "textmods/TextState.h"

etm::TextBuffer::pos::pos(): pos(0, 0) {
}
etm::TextBuffer::pos::pos(lines_number_t row, line_index_t column):
    row(row), column(column) {
}

etm::TextBuffer::TextBuffer(Resources *res, Scroll &scroll, line_index_t width):
    res(res), scroll(&scroll),
    width(width), dispCursor(res),
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

void etm::TextBuffer::pushMod(const std::shared_ptr<tm::Mod> &mod) {
    modifierBlocks.push_back(mod);
    modifierBlocks_t::size_type index = modifierBlocks.size() - 1;
    Line::string_t str;
    str.push_back(env::CONTROL_CHAR);
    int shift = 8 * static_cast<int>(env::CONTROL_BLOCK_SIZE);
    for (unsigned int n = 0; n < env::CONTROL_BLOCK_SIZE; n++) {
        shift -= 8;
        str.push_back(static_cast<Line::value_type>((index >> shift) & 0xff));
    }
    if (!lines.size()) {
        newline();
    }
    lines.back().appendControl(str);
}

void etm::TextBuffer::clear() {
    lines.clear();
    modifierBlocks.clear();
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

    // long size = static_cast<long>(sizeof(Line) * lines.capacity() + sizeof(std::vector<Line>));
    // for (Line &l : lines) {
    //     size += static_cast<long>(l.string.capacity() * sizeof(Line::value_type));
    // }

    // std::cout << "MEMORY USAGE: " << (size / 1e6) << std::endl;
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

void etm::TextBuffer::doAppend(Line::value_type c) {
    if (c == '\n') {
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
        if (c != ' ' && (lastLine.size() - 1 > lastLine.size() || lastLine[lastLine.size()-1] != ' ')) {
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

void etm::TextBuffer::write(lines_number_t row, line_index_t column, Line::value_type c) {
    if (outOfBounds(row, column)) return;

    if (column == lines[row].size()-1 && lines[row].hasNewline() && c != '\n') {
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
        lines[row].eraseChar(column);
        reformat(row, column-1);
    }

}

void etm::TextBuffer::append(Line::value_type c) {
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

void etm::TextBuffer::insertAtCursor(Line::value_type c) {
    if (cursorAtEnd()) {
        doAppend(c);
    } else {
        insert(cursor.row, cursor.column, c);
    }
    moveCursorCollumnWrap(1);
}

void etm::TextBuffer::insert(lines_number_t row, line_index_t column, Line::value_type c) {
    if (outOfBounds(row, column)) return;

    if (c == '\n') {
        if (!lines[row].hasNewline()) {            
            // Move the text after the newline to the newly created line
            Line::string_t buffer(lines[row].substr(column));
            lines[row].erase(column);
            lines[row].setNewline(true);
            if (row + 1 >= lines.size()) {
                newline();
                lines[row+1].appendStr(buffer);
            } else {
                lines[row+1].prependStr(buffer);
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
            buffer.push_back('\n');
        }
    }
    lines.erase(lines.begin() + row + 1, lines.end());
    for (Line::value_type &c : buffer) {
        doAppend(c);
    }
}

void etm::TextBuffer::bindChar(Line::value_type c) {
    textCache_t::iterator loc = textCache.find(c);
    if (loc != textCache.end()) {
        loc->second.bind();
    } else {
        textCache[c] = res->getFont().renderChar(c);
        textCache[c].bind();
    }
}

void etm::TextBuffer::applyMod(Line::size_type ctrlIndex, Line &line, tm::TextState &state) {
    int value = 0;
    int shift = 8 * static_cast<int>(env::CONTROL_BLOCK_SIZE);
    for (Line::size_type i = ctrlIndex+1, end = i + env::CONTROL_BLOCK_SIZE; i < end; i++) {
        shift -= 8;
        value |= line.getDejure(i) << shift;
    }
    modifierBlocks[value]->run(state);
}

void etm::TextBuffer::render(int x, int y) {

    if (cursorEnabled && displayCursor) {
        // Assume that the primitive shader was already set by
        // the terminal
        dispCursor.setX(static_cast<int>(cursor.column * charWidth()));
        dispCursor.setY(y + static_cast<int>(cursor.row * charHeight()));
        dispCursor.setHeight(charHeight());
        dispCursor.render();
    }

    res->bindTextShader();

    lines_number_t start = std::floor(scroll->getOffset() / charHeight());
    lines_number_t end = std::min(
        lines.size(),
        static_cast<lines_number_t>(
            start + scroll->getNetHeight() / charHeight() + 1
        )
    );

    Model model(x, y + static_cast<int>(charHeight() * start), charWidth(), charHeight());

    tm::TextState state(res->getShader(), defBackgroundColor, defForegroundColor);

    // Move back to get color settings that might've been
    // missed had we started at `start`.
    // This does make me mad through, because it means that the
    // time complexity is linear as opposed to constant...
    // Well, it's definitely better than the alternatives...
    // The goal is to prioritize memory... Performance is less of an
    // issue, since the console itn't going to be active, _all the time_
    for (lines_number_t r = 0; r < start; r++) {
        line_t &line = lines[r];
        for (line_index_t c = 0; c < line.dejureSize(); c++) {
            Line::value_type chr = line.getDejure(c);
            if (chr == env::CONTROL_CHAR) {
                applyMod(c, line, state);
                c += env::CONTROL_BLOCK_SIZE;
            }
        }
    }

    for (lines_number_t r = start; r < end; r++) {
        line_t &line = lines[r];
        for (line_index_t c = 0; c < line.dejureSize(); c++) {
            Line::value_type chr = line.getDejure(c);
            if (chr == env::CONTROL_CHAR) {
                applyMod(c, line, state);
                c += env::CONTROL_BLOCK_SIZE;
            } else {
                bindChar(chr);
                model.set(res->getShader());
                res->renderRectangle();
                model.x += charWidth();
            }
        }
        model.x = 0;
        model.y += charHeight();
    }
}
