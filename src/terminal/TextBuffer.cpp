#include "TextBuffer.h"

#include <iostream>
#include <cmath>
#include <iterator>

#include "gui/Rectangle.h"
#include "Resources.h"
#include "Scroll.h"
#include "textmods/TextState.h"
#include "textmods/RenderState.h"
#include "textmods/Lookbehind.h"

static constexpr etm::TextBuffer::lines_number_t DEF_MAX_NUMBER_LINES = 1000; 
static const std::string nullCodepointStr("\0");
static const etm::Line::codepoint nullCodepoint(nullCodepointStr.begin(), nullCodepointStr.end());

// Check if the char is valid (ie not a reserved
// one).
// If it's not, sets it to something that is valid.
static void filterChar(etm::Line::codepoint &c);

void filterChar(etm::Line::codepoint &c) {
    if (c == etm::env::CONTROL_CHAR_START || c == etm::env::CONTROL_CHAR_END) {
        c = nullCodepoint;
    }
}

etm::TextBuffer::pos::pos(): pos(0, 0) {
}
etm::TextBuffer::pos::pos(lines_number_t row, line_index_t column):
    row(row), column(column) {
}

etm::TextBuffer::TextBuffer(Resources *res, Scroll &scroll, line_index_t width):
    res(res), scroll(&scroll),
    maxNumberLines(DEF_MAX_NUMBER_LINES),
    width(width), dispCursor(res),
    dfSelectStart(&selectStart), dfSelectEnd(&selectEnd),
    cursorEnabled(false), displayCursor(false),
    blockId(0)
{
    setDefForeGColor(0xffffff);
    setDefBackGColor(0x000000);
    setCursorColor(0xffffff);
    setCursorWidth(1);
}

void etm::TextBuffer::checkNumberLines() {
    while (lines.size() > maxNumberLines) {
        deleteFirstLine();
        // Decrement cursor to account for loss of rows
        cursorMin.row--;
        cursor.row--;
    }
}

void etm::TextBuffer::newline() {
    lines.emplace_back();
    checkNumberLines();
}

void etm::TextBuffer::insertNewline(line_index_t row) {
    lines.insert(lines.begin() + row + 1, line_t());
    checkNumberLines();
}

void etm::TextBuffer::deleteLastLine() {
    int countCtrl = 0;
    for (line_index_t i = 0; i < lines.back().dejureSize(); i++) {
        if (lines.back().getDejure(i) == env::CONTROL_CHAR_START) {
            i += env::CONTROL_BLOCK_SIZE;
            countCtrl++;
        }
    }
    lines.pop_back();
    // Order of the mod blocks is the same as the lines
    modifierBlocks_t::iterator it = modifierBlocks.end();
    std::advance(it, -countCtrl);
    modifierBlocks.erase(it, modifierBlocks.end());
}

void etm::TextBuffer::deleteFirstLine() {
    int countCtrl = 0;
    for (line_index_t i = 0; i < lines.front().dejureSize(); i++) {
        if (lines.front().getDejure(i) == env::CONTROL_CHAR_START) {
            i += env::CONTROL_BLOCK_SIZE;
            countCtrl++;
        }
    }
    lines.erase(lines.begin());
    // Order of the mod blocks is the same as the lines
    modifierBlocks_t::iterator it = modifierBlocks.begin();
    std::advance(it, countCtrl);
    modifierBlocks.erase(modifierBlocks.begin(), it);
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

bool etm::TextBuffer::isStartSpace(const Line::codepoint &c, lines_number_t row) {
                        // because it's unsigned
    return c == ' ' && row - 1 < lines.size() && lines[row - 1][lines[row - 1].size() - 1] != ' ';
}

void etm::TextBuffer::pushMod(const std::shared_ptr<tm::Mod> &mod) {
    modifierBlocks[blockId] = mod;
    Line::string_t str;
    str.push_back(env::CONTROL_CHAR_START);
    int shift = 8 * env::CONTROL_BLOCK_DATA_BYTES;
    for (unsigned int n = 0; n < env::CONTROL_BLOCK_DATA_BYTES; n++) {
        shift -= 8;
        str.push_back(static_cast<Line::value_type>((blockId >> shift) & 0xff));
    }
    str.push_back(env::CONTROL_CHAR_END);
    if (!lines.size()) {
        newline();
    }
    lines.back().appendControl(str);
    blockId++;
}

void etm::TextBuffer::clear() {
    lines.clear();
    modifierBlocks.clear();
    res->getFont().clearCache();
    newline();
    jumpCursor();
}

void etm::TextBuffer::setMaxLines(lines_number_t count) {
    maxNumberLines = count;
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

void etm::TextBuffer::doAppend(const Line::codepoint &c) {
    if (!lines.size()) {
        newline();
    }

    if (c == '\n') {
        std::string str;
        lines.back().copyTo(str);
        lines.back().setNewline(true);
        newline();
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
            Line::iterator it(lastLine.last());
            it -= 2;
            for (; it.valid(); --it) {
                if (*it == ' ') {
                    ++it;
                    const Line::iterator eraseOffset = it;
                    // Move the last word from the last line to the next
                    // line via recursion
                    for (; it.valid(); ++it) {
                        append(*it);
                    }

                    lastLine.erase(eraseOffset);
                    break;
                }
            }
            append(c);
        } else if (isStartSpace(c, lines.size() - 1)) {
            nextLine.setStartSpace(true);
        } else {
            // Prevent stack overflow from width of 0...
            // should never happen, but I like to have a reason
            // to avoid recursion
            nextLine.appendChar(c);
        }
    } else {
        lines.back().appendChar(c);
    }
}

void etm::TextBuffer::eraseAtCursor() {
    lines_number_t row = cursor.row;
    line_index_t column = cursor.column - 1;
    // Because it's unsigned it underflows to big number
    if (column >= lines[row].size()) {
        row--;
        // Because it's unsigned it underflows to big number
        if (row < lines.size() && row >= cursorMin.row) {
            column = lines[row].size() - 1;
        } else {
            return;
        }
    }
    if ((column >= cursorMin.column || row > cursorMin.row) && row >= cursorMin.row && column < lines[row].size()) {
        if (cursorAtEnd()) {
            prepare();
            doTrunc();
            jumpCursor();
        } else {
            // Didn't explicitly check that was in bounds
            erase(row, column);
            cursor.column = column;
            cursor.row = row;
        }
    }
}

void etm::TextBuffer::erase(lines_number_t row, line_index_t column) {
    if (!outOfBounds(row, column)) {
        prepare();
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
    } else if (column == 0 && lines[row].hasStartSpace()) {
        lines[row].setStartSpace(false);
        reformat(row - 1 < row ? row - 1 : 0, 0);
    } else {
        lines[row].eraseChar(column);
        // unsigned madness
        reformat(row - 1 < row ? row - 1 : 0, 0);
        // reformat(row, 0);
    }

}

void etm::TextBuffer::append(Line::codepoint c) {
    filterChar(c);
    const bool moveCursor = cursorAtEnd();
    doAppend(c);
    if (moveCursor) {
        jumpCursor();
    }
}

void etm::TextBuffer::trunc() {
    prepare();
    const bool moveCursor = cursorAtEnd();
    doTrunc();
    if (moveCursor) {
        jumpCursor();
    }
}

void etm::TextBuffer::doTrunc() {

    if (lines.size()) {
        if (!lines.back().size()) {
            if (!lines.back().hasStartSpace()) {
                // If line empty, delete it
                if (lines.size() > 1) {
                    // Only if this isn't the last line
                    deleteLastLine();
                } else {
                    // This is the last line and it has no chars.
                    // Deny everything.
                    return;
                }
            } else {
                // If empty with the startspace, just "delete" the
                // start space instead and be done with this
                lines.back().setStartSpace(false);
                return;
            }
        }
        lines.back().popBack();
        // Don't delete line if it's the only one.
        if (lines.size() > 1 && !lines[lines.size() - 2].hasNewline()) {
            // Delete line if empty and last line didn't force
            // a newline break.
            if (!lines.back().size()) {
                deleteLastLine();
            } else if (width - lines[lines.size()-2].size() >= lines.back().size()) {
                // Move data, as it can sort-of reverse-wrap back to the previous line
                lines[lines.size()-2].appendOther(lines.back());
                deleteLastLine();
            }
        }
    }

}

void etm::TextBuffer::insertAtCursor(Line::codepoint c) {
    prepare();
    if (lines.size()) {
        filterChar(c);
        int cursorMove = 1;
        if (cursorAtEnd()) {
            doAppend(c);
        } else {
            if (cursor.column == lines.back().size()) {
                // Already asserted that this isn't the last line
                // with cursorAtEnd()
                cursor.row++;
                if (lines[cursor.row].hasStartSpace()) {
                    lines[cursor.row].setStartSpace(false);
                    lines[cursor.row].insertChar(0, ' ');
                    lines[cursor.row].insertChar(0, c);
                    reformat(cursor.row - 1, 0);
                    cursorMove = 2;
                } else {
                    insert(cursor.row + 1, 0, c);
                }
            } else {
                insert(cursor.row, cursor.column, c);
            }
        }
        moveCursorCollumnWrap(cursorMove);
    }
}

void etm::TextBuffer::insert(lines_number_t row, line_index_t column, Line::codepoint c) {
    if (!outOfBounds(row, column)) {
        filterChar(c);
        prepare();
        doInsert(row, column, c);
    }
}
void etm::TextBuffer::doInsert(lines_number_t row, line_index_t column, const Line::codepoint &c) {

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
    } else if (isStartSpace(c, row) && column == 0 && !lines[row].hasStartSpace()) {
        lines[row].setStartSpace(true);
        // because unsigned
        reformat(row - 1 < row ? row - 1 : 0, 0);
    } else {
        lines[row].insertChar(column, c);
        if (lines[row].size() > width) {
            reformat(row, column);
        }
    }

}

void etm::TextBuffer::reformat(lines_number_t row, line_index_t column) {
    // We assume that row and column are valid, and that there are > 0 lines
    Line::string_t buffer(lines[row].substr(column));
    lines[row].erase(column);
    if (lines[row].hasNewline()) {
        buffer.push_back('\n');
        lines[row].setNewline(false);
    }
    for (lines_number_t r = row+1; r < lines.size(); r++) {
        if (lines[r].hasStartSpace()) {
            buffer.push_back(' ');
        }
        lines[r].copyTo(buffer);
        if (lines[r].hasNewline()) {
            buffer.push_back('\n');
        }
    }
    lines.erase(lines.begin() + row + 1, lines.end());
    for (std::string::size_type i = 0; i < buffer.size();) {
        if (buffer[i] == env::CONTROL_CHAR_START) {
            lines.back().appendControlFromRange(buffer, i, env::CONTROL_BLOCK_SIZE + 1);
            i += env::CONTROL_BLOCK_SIZE + 1;
        } else {
            const int size = utf8::test(buffer[i]);
            Line::codepoint cp(buffer.begin() + i, buffer.begin() + (i + size));
            doAppend(cp);
            i += size;
        }
    }
}

void etm::TextBuffer::clampPos(pos &p, lines_number_t row, line_index_t column) {
    if (lines.size()) {
        if (row < lines.size()) {
            p.row = row;
            p.column = std::min(column, lines[p.row].size());
        } else {
            p.row = lines.size() - 1;
            p.column = lines[p.row].size();
        }
    } else {
        p.row = 0;
        p.column = 0;
    }
}

void etm::TextBuffer::initSelection(lines_number_t row, line_index_t column) {
    clampPos(selectStart, row, column);
    selectEnd = selectStart;
    // Order of select start/end don't matter now, as they're the same
}
void etm::TextBuffer::setSelectionEnd(lines_number_t row, line_index_t column) {
    clampPos(selectEnd, row, column);
    // If the end is before the start, swap the two
    if (selectEnd.row < selectStart.row || (selectEnd.row == selectStart.row && selectEnd.column < selectStart.column)) {
        dfSelectStart = &selectEnd;
        dfSelectEnd = &selectStart;
    } else {
        dfSelectStart = &selectStart;
        dfSelectEnd = &selectEnd;
    }
}

std::string etm::TextBuffer::getSelectionText() {
    return getTextFromRange(*dfSelectStart, *dfSelectEnd);
}

std::string etm::TextBuffer::getTextFromRange(const pos &start, const pos &stop) {
    if (start.row > stop.row || (start.row == stop.row && start.column > stop.column)) {
        return "";
    }
    std::string buffer;
    if (start.row == stop.row) {
        // Midsection
        for (
            Line::iterator it = lines[start.row].begin() + start.column,
            end = it + (stop.column - start.column);
            it.valid() && it < end;
            ++it
            )
        {
            Line::codepoint c(*it);
            buffer.append(c.start, c.end);
        }
    } else {
        // End of first line
        for (
            Line::iterator it = lines[start.row].begin() + start.column;
            it.valid();
            ++it
            )
        {
            Line::codepoint c(*it);
            buffer.append(c.start, c.end);
        }
        if (lines[start.row].hasNewline()) {
            buffer.push_back('\n');
        }
        // Every line in-between
        for (lines_number_t l = start.row + 1; l < stop.row; l++) {
            if (lines[l].hasStartSpace()) {
                buffer.push_back(' ');
            }
            for (Line::iterator it = lines[l].begin(); it.valid(); ++it) {
                Line::codepoint c(*it);
                buffer.append(c.start, c.end);
            }
            if (lines[l].hasNewline()) {
                buffer.push_back('\n');
            }
        }
        // Start of last line
        if (lines[stop.row].hasStartSpace()) {
            buffer.push_back(' ');
        }
        for (
            Line::iterator it = lines[stop.row].begin(),
            end = it + stop.column;
            it.valid() && it < end;
            ++it
            )
        {
            Line::codepoint c(*it);
            buffer.append(c.start, c.end);
        }
    }

    return buffer;
}

std::string etm::TextBuffer::pollInput() {
    if (!lines.size()) {
        newline();
    }
    return getTextFromRange(cursorMin, pos(lines.size() - 1, lines.back().size()));
}

void etm::TextBuffer::prepare() {
    // Remove selection
    selectStart.row = 0;
    selectStart.column = 0;
    selectEnd.row = 0;
    selectEnd.column = 0;
}

etm::TextBuffer::mod_t &etm::TextBuffer::getMod(Line::size_type ctrlIndex, Line &line) {
    env::type value = 0;
    int shift = 8 * static_cast<int>(env::CONTROL_BLOCK_DATA_BYTES);
    for (Line::size_type i = ctrlIndex+1, end = i + env::CONTROL_BLOCK_DATA_BYTES; i < end; i++) {
        shift -= 8;
        // Really fucking weird.
        // static_cast<int>(char) will return the char WITH THE SIGN.
        // so -128 will be -128.
        // WHAT.
        // Well I guess it makes sense from a static_cast perspective.
        // I was used to the reinterpret_cast kinda' deal from Java,
        // or whatever it uses.
        value |= static_cast<env::type>(static_cast<unsigned char>(line.getDejure(i))) << shift;
    }
    return modifierBlocks[value];
}

void etm::TextBuffer::render(int x, int y) {

    // "After scrollbar renders, the text shader is set
    // due to the rendering of the triangle textures,
    // so display doesn't need to set it."
    // - Comment right after the scrollbar render call in Terminal
    // res->bindTextShader();

    lines_number_t start = std::floor(scroll->getOffset() / charHeight());
    lines_number_t end = std::min(
        lines.size(),
        static_cast<lines_number_t>(
            start + scroll->getNetHeight() / charHeight()
        )
    );

    Model model(x, y + static_cast<int>(charHeight() * start), charWidth(), charHeight());

    tm::Lookbehind lookbehind(defBackgroundColor, defForegroundColor, start - 1);

    // Move back to get color settings that might've been
    // missed had we started at `start`.
    // This does make me mad through, because it means that the
    // time complexity is linear as opposed to constant...
    // Well, it's definitely better than the alternatives...
    // The goal is to prioritize memory... Performance is less of an
    // issue, since the console itn't going to be active, _all the time_
    for (lines_number_t r = start - 1; r < start && !lookbehind.bothSet(); r--) {
        line_t &line = lines[r];
        for (line_index_t c = 0; c < line.dejureSize(); c++) {
            Line::value_type chr = line.getDejure(c);
            if (chr == env::CONTROL_CHAR_START) {
                getMod(c, line)->run(lookbehind);
                c += env::CONTROL_BLOCK_SIZE;
            }
        }
        lookbehind.decLine();
    }

    tm::RenderState state(
        res->getShader(),
        lookbehind.getBack(),
        lookbehind.getFore(),
        selectStart.row < start && start < selectEnd.row
    );

    for (lines_number_t r = start; r < end; r++) {
        line_t &line = lines[r];
        if (r == dfSelectStart->row && 0 == dfSelectStart->column) {
            state.setInverted(true);
        }
        // Can be the same, in which case they negate each-other
        if (r == dfSelectEnd->row && 0 == dfSelectEnd->column) {
            state.setInverted(false);
        }
        // c is the defacto index, one representing the characters
        // irrespective of the byte size.
        // cc is the dejure index, the actual byte offset.
        for (line_index_t c = 0, cc = 0; c < line.dejureSize();) {
            Line::value_type chr = line.getDejure(c);
            if (chr == env::CONTROL_CHAR_START) {
                getMod(c, line)->run(state);
                c += env::CONTROL_BLOCK_SIZE + 1;
            } else {
                const int size = utf8::test(chr);

                res->getFont().bindChar(utf8::read(line.getString(), c, size));
                model.set(res->getShader());
                res->renderRectangle();
                model.x += charWidth();

                c += size;
                cc++;

                if (r == dfSelectStart->row && cc == dfSelectStart->column) {
                    state.setInverted(true);
                }
                // Can be the same, in which case they negate each-other
                if (r == dfSelectEnd->row && cc == dfSelectEnd->column) {
                    state.setInverted(false);
                }
            }
        }
        model.x = 0;
        model.y += charHeight();
    }

    // Unfortunately, we have to render the cursor after,
    // triggering another shader change else
    // the text background will hide it :(
                                        // cuz' unsigned, also
                                        // cursor can only ever be
                                        // at or past the end
    if (cursorEnabled && displayCursor && cursor.row < end) {
        res->bindPrimitiveShader();
        dispCursor.setX(x + static_cast<int>(cursor.column * charWidth()));
        dispCursor.setY(y + static_cast<int>(cursor.row * charHeight()));
        dispCursor.setHeight(charHeight());
        dispCursor.render();
    }
}
