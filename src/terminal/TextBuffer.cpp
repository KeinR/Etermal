#include "TextBuffer.h"

#include <cmath>

#include "gui/Rectangle.h"
#include "Resources.h"
#include "Scroll.h"
#include "textmods/TextState.h"
#include "textmods/RenderState.h"
#include "textmods/Lookbehind.h"

static constexpr etm::TextBuffer::lines_number_t DEF_MAX_NUMBER_LINES = 1000; 
static const std::string nullCodepointStr("\0");
static const etm::Line::codepoint nullCodepoint(nullCodepointStr.begin(), nullCodepointStr.end());

/**
* Check if the codepoint is valid (not a reserved one)
* If it's not valid, sets it to something that is valid.
* @param [in,out] c The codepoint to filter
*/
static void filterChar(etm::Line::codepoint &c);

void filterChar(etm::Line::codepoint &c) {
    if (etm::ctrl::testStart(*c.start) || etm::ctrl::testEnd(*c.start)) {
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
    cursorEnabled(false), displayCursor(false)
{
    setDefForeGColor(0xffffff);
    setDefBackGColor(0x000000);
    setCursorColor(0xffffff);
    setCursorWidth(1);
}

void etm::TextBuffer::setScroll(Scroll &scroll) {
    this->scroll = &scroll;
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
        if (ctrl::testStart(lines.back().getDejure(i))) {
            i += ctrl::getJump();
            countCtrl++;
        }
    }
    lines.pop_back();
    modifierBlocks.eraseBack(countCtrl);
}

void etm::TextBuffer::deleteFirstLine() {
    int countCtrl = 0;
    for (line_index_t i = 0; i < lines.front().dejureSize(); i++) {
        if (ctrl::testStart(lines.front().getDejure(i))) {
            i += ctrl::getJump();
            countCtrl++;
        }
    }
    lines.erase(lines.begin());
    modifierBlocks.eraseFront(countCtrl);
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
    Line::string_t str = ctrl::encode(modifierBlocks.add(mod));
    if (!lines.size()) {
        newline();
    }
    lines.back().appendControl(str);
}

void etm::TextBuffer::clear() {
    lines.clear();
    modifierBlocks.clear();
    res->getFont().clearCache();
    newline();
    jumpCursor();
    lines.shrink_to_fit();
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
bool etm::TextBuffer::moveCursorRow(int distance) {
    lines_number_t row = cursor.row;
    cursor.row = std::min(std::max(cursor.row + distance, cursorMin.row), lines.size()-1);
    cursor.column = std::min(cursor.column, lines[cursor.row].size());
    return row != cursor.row;
}
void etm::TextBuffer::moveCursorCollumnWrap(int distance) {
    // Signed value so that we can detect underflow (negatives)
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
        // Wrap cursor to next line (if it exists) if at the end of _this_ line.
        // I mean MinTTY does it so really I have no choice.
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
    if (lines.size()) {
        reformat(0, 0);
    }
}
etm::TextBuffer::line_index_t etm::TextBuffer::getWidth() {
    return width;
}

void etm::TextBuffer::doAppend(const Line::codepoint &c) {
    if (!lines.size()) {
        newline();
    }

    if (c == '\n') {
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
            Line::iterator it(lastLine.last());
            // Don't decrement by 2 because Line::last() is the last char (Line::size() - 1) 
            --it;
            for (; it.valid(); --it) {
                if (*it == ' ') {
                    ++it;
                    const Line::iterator eraseOffset = it;
                    // Move the last word from the last line to the next
                    // line via recursion
                    for (; it.valid(); ++it) {
                        doAppend(*it);
                    }

                    lastLine.erase(eraseOffset);
                    break;
                }
            }
            doAppend(c);
        } else if (isStartSpace(c, lines.size() - 1)) {
            nextLine.setStartSpace(true);
        } else {
            // Prevent stack overflow from `width` being 0...
            // should never happen, but there's no reason
            // to not desire to not not avoid recursion
            nextLine.appendChar(c);
        }
    } else {
        lines.back().appendChar(c);
    }
}

void etm::TextBuffer::eraseAtCursor() {
    if (cursor.column == 0 && lines[cursor.row].hasStartSpace()) {
        // In this case, we don't want to move backwards - 
        // instead, we just want to delete the start space.

        // This is spitting in the face of erase(...) a little
        // bit since it already does this, but there's no real reason
        // why we need to go through and invoke erase(...) itself,
        // given we know what we're doing.
        lines[cursor.row].setStartSpace(false);

        // The only time when a start space can be applied is when the
        // line is completely filled up, the last character on the last
        // line was NOT a space nor a newline char.
        // Once the start space is removed, there's a chance that a (possible) word
        // on this line will conjoin with the one that terminated the last line.
        // If it does, the size of this line will increase due to the larger word
        // getting wrapped.
        // Given the preconditions, it is impossible that this line could lose
        // any size; however, it is possible that this line increases in size,
        // if it starts with a character that isn't a space.
        // While we could check if this line has a space and only
        // modify the position that way, this solution feels more versitile:
        // increment the cursor column by the line's increase in size.
        line_index_t prevSize = lines[cursor.row].size();
        reformat(cursor.row - 1 < cursor.row ? cursor.row - 1 : 0, 0);
        cursor.column += lines[cursor.row].size() - prevSize;
    } else {
        // Check if the cursor can move back one, deleting the
        // previous char
        lines_number_t row = cursor.row;
        // Erase the codepoint before the cursor
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
                // Better to just truncate if at the end
                trunc();
            } else {
                // Didn't explicitly check that was in bounds,
                // so call erase(), which does bounds checks
                erase(row, column);
                // If we have a situation where spaces take up
                // 55/60 columns, and there's a word of length 4,
                // followed by a 5 length word that's been wrapped,
                // if the space in-between them is deleted then the 4
                // and 5 length words will conjoin to form a 9 length
                // word that is wrapped to the next line.
                // The cursor will however not necessarily know this,
                // and without doing anything else it will stay in an invalid
                // column of 60 (zero-indexed), while the length of that line
                // is actually 55 and the position it should be in is after the 4th
                // character in the 9 length word that was wrapped to the next
                // line.

                cursor.column = column;
                cursor.row = row;

                // This is a little dangerous I'll admit, but the
                // only situation where this would be the case
                // would be if the wrapping changed and text was pushed
                // to the next line.
                if (cursor.column > lines[row].size()) {
                    cursor.column = column - lines[row].size();
                    cursor.row++;
                }
            }
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

    if (column == lines[row].size()-1 && lines[row].hasNewline()) {
        // If was the last char and the line was broken manually:
        // instead just delete the newline
        lines[row].setNewline(false);
        reformat(row, column);
    } else if (column == 0 && lines[row].hasStartSpace()) {
        // Delete start space if in position.
        lines[row].setStartSpace(false);
        // Reformat from the last line because could've
        // made it so that a word at the end is too small
        // to be soft wrapped.
        // "less than self" because it's unsigned.
        reformat(row - 1 < row ? row - 1 : 0, 0);
    } else {
        lines[row].eraseChar(column);
        reformat(row - 1 < row ? row - 1 : 0, 0);
    }
}

void etm::TextBuffer::append(Line::codepoint c) {
    filterChar(c);
    doAppend(c);
    // Cursor should always be jumpped when appending.
    // If this is not the desired behavior (ex when
    // reformatting), call doAppend()
    jumpCursor();
}

void etm::TextBuffer::trunc() {
    prepare();
    doTrunc();
    // Cursor should always be jumpped when truncating.
    // If this is not the desired behavior (ex when
    // reformatting), call doTrunc()
    jumpCursor();
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
            } else if (width - lines[lines.size() - 2].size() >= lines.back().size()) {
                // Move data if there's space, as it can sort-of reverse-wrap back to the previous line
                lines[lines.size()-2].appendOther(lines.back());
                deleteLastLine();
            }// else {
                // The content change didn't decrease the size of the
                // line enough to cause the line to wrap back to the previous.
                // We don't have to do a reformat(...) because the last 
                // reformatting garuntees that the lines are wrapped properly,
                // meaning that we don't have to worry about the effects on words
                // disconnected from the start word, and and you can only wrap
                // the word back if there's enough space on the last line.
            //}
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
            // The cursor will be stopped from resting after the
            // last character of a line, if that line isn't the last line.
            // Rather, it is pushed to before the first char of the next
            // line (first index).
            // If the last line is terminated by spaces, its size is less
            // than the max column size, and this line is started by a non-space
            // character, then the cursor must not move, as the space would end
            // up inserted at the last line, and there would be no changes to the word
            // wrapping because spaces are what induce word wrapping.
            if (
                lines[cursor.row].size() > 0 && lines[cursor.row][0] != ' ' &&
                cursor.row - 1 < lines.size() && lines[cursor.row - 1].size() < width &&
                lines[cursor.row - 1].size() > 0 && lines[cursor.row - 1][lines[cursor.row - 1].size() - 1] == ' '
                )
            {
                cursorMove = 0;
            }

            insert(cursor.row, cursor.column, c);
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
        reformat(row - 1 < row ? row - 1 : 0, 0);
    }

}

void etm::TextBuffer::reformat(lines_number_t row, line_index_t column) {
    // We assume that row and column are valid, and that there are > 0 lines.
    // Copy all the data after and including the given position,
    // then delete the lines that the data came from and re-append everything.
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
        if (ctrl::testStart(buffer[i])) {
            lines.back().appendControlFromRange(buffer, i, ctrl::getJump() + 1);
            i += ctrl::getJump() + 1;
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
                                        // last char of string defined
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
    // If the end is before the start, swap the two.
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
    pos endpoint;
    clampPos(endpoint, stop.row, stop.column);
    // Denies if start > end, so we're fine
    return doGetTextFromRange(start, endpoint);
}

std::string etm::TextBuffer::doGetTextFromRange(const pos &start, const pos &stop) {
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
        return "";
    }
    return doGetTextFromRange(cursorMin, pos(lines.size() - 1, lines.back().size()));
}

void etm::TextBuffer::clearInput() {
    int brea = 3;
    brea++;
    lines[cursorMin.row].erase(cursorMin.column);
    lines.erase(lines.begin() + cursorMin.row + 1, lines.end());
}

void etm::TextBuffer::prepare() {
    // Remove selection
    selectStart.row = 0;
    selectStart.column = 0;
    selectEnd.row = 0;
    selectEnd.column = 0;
}

etm::TextBuffer::mod_t &etm::TextBuffer::getMod(Line::size_type ctrlIndex, Line &line) {
    return modifierBlocks.get(ctrl::decode(line.getString().begin() + ctrlIndex, line.getString().end()));
}

void etm::TextBuffer::getRange(lines_number_t &start, lines_number_t &end) {
    start = std::floor(scroll->getOffset() / charHeight());
    end = std::min(
        lines.size(),
        static_cast<lines_number_t>(
            start + scroll->getNetHeight() / charHeight()
        )
    );
}

void etm::TextBuffer::render(int x, int y) {

    // "After scrollbar renders, the text shader is set
    // due to the rendering of the triangle textures,
    // so display doesn't need to set it."
    // - Comment right after the scrollbar render call in Terminal
    // res->bindTextShader();

    // Correct the y-offset with the scroll offset
    y -= scroll->getOffset();

    // Only render the range that is visible
    lines_number_t start, end;
    getRange(start, end);

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
            if (ctrl::testStart(chr)) {
                getMod(c, line)->run(lookbehind);
                c += ctrl::getJump();
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

    Model model(x, y + static_cast<int>(charHeight() * start), charWidth(), charHeight());

    for (lines_number_t r = start; r < end; r++) {
        line_t &line = lines[r];
        if (r == dfSelectStart->row && 0 == dfSelectStart->column) {
            state.setInverted(true);
        }
        // Can be the same, in which case they negate each-other
        if (r == dfSelectEnd->row && 0 == dfSelectEnd->column) {
            state.setInverted(false);
        }
        // c is the deFacto index, one representing the characters
        // irrespective of the byte size.
        // cc is the deJure index, the actual byte offset.
        for (line_index_t c = 0, cc = 0; c < line.dejureSize();) {
            Line::value_type chr = line.getDejure(c);
            if (ctrl::testStart(chr)) {
                getMod(c, line)->run(state);
                c += ctrl::getJump() + 1;
            } else {
                const int size = utf8::test(chr);

                res->getFont().bindChar(utf8::read(line.getString(), c, size));
                model.set(res);
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
}

void etm::TextBuffer::renderCursor(int x, int y) {
    if (cursorEnabled && displayCursor) {
        lines_number_t start, end;
        getRange(start, end);
        // cuz' unsigned, also cursor can only
        // ever be at or past the end
        if (cursor.row < end) {
            res->bindPrimitiveShader();
            dispCursor.setX(x + static_cast<int>(cursor.column * charWidth()));
            dispCursor.setY(y - scroll->getOffset() + static_cast<int>(cursor.row * charHeight()));
            dispCursor.setHeight(charHeight());
            dispCursor.render();
        }
    }
}

