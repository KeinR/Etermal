#include "Terminal.h"

#include <iostream>
#include <algorithm>
#include <cmath>

#include "util/termError.h"
#include "render/opengl.h"
#include "../TermInput.h"
#include "../EShell.h"
#include "State.h"
#include "codec.h"
#include "Line.h"
#include "Resources.h"
#include "util/debug.h"

#include "textmods/Mod.h"
#include "textmods/mods.h"

/**
* The default error callback.
* @param [in] error The error object
*/
static void defaultErrorCallback(const etm::termError &error);

void defaultErrorCallback(const etm::termError &error) {
    std::cerr
        << "---------------------------\n"
        << "ETERMAL::TERMINAL::ERROR:\n"
        << "location = " << error.location << "\n"
        << "code = 0x" << std::hex << error.code << std::dec << " (dec = " << error.code << ")" << "\n"
        << "severe = " << (error.severe ? "TRUE" : "FALSE") << "\n"
        << "message = \"" << error.message << "\"\n"
        << "---------------------------\n";
}

etm::Terminal::Terminal(bool postponeInit):
    Terminal(defaultErrorCallback, postponeInit)
{
}
etm::Terminal::Terminal(const std::shared_ptr<EtmFont> &font, bool postponeInit):
    Terminal(defaultErrorCallback, postponeInit)
{
    setFont(font);
}
etm::Terminal::Terminal(const errCallback_t &errorCallback, const std::shared_ptr<EtmFont> &font, bool postponeInit):
    // Make sure that the provided callback is valid
    Terminal(errorCallback ? errorCallback : defaultErrorCallback, postponeInit)
{
    setFont(font);
}
etm::Terminal::Terminal(const errCallback_t &errorCallback, bool postponeInit):
    errorCallback(errorCallback),
    resources(new Resources(*this)),
    viewport(0, 0, 100, 100),
    scroll(resources),
    scrollbar(resources, scroll),
    scrollSensitivity(25.0f),
    display(resources, scroll, 30),
    background(resources),
    focused(true),
    takeInput(false),
    escapeNext(false),
    cursorBlink(500),
    shell(nullptr),
    dragging(false),
    framebufValid(false),
    isInit(false)
{
    if (!postponeInit) {
        init();
    }

    setBackgroundColor(0x080808);
    setTextColor(0xf0f0f0);

    scrollbar.setWidth(22);
    scrollbar.setSideMargin(2);
    scrollbar.setSliderColor(0xbababa);
    scrollbar.setBarColor(0xf5f5f5);

    // Wait for the shell to tell us that it wants input
    display.setCursorEnabled(false);

    if (!postponeInit) {
        updatePosition();
    }
}
etm::Terminal::~Terminal() {
    if (resources != nullptr) {
        delete resources;
    }
}
etm::Terminal::Terminal(Terminal &&other):
// This is a terrible solution.
errorCallback(std::move(other.errorCallback)),
resources(std::move(other.resources)),
viewport(std::move(other.viewport)),
scroll(std::move(other.scroll)),
scrollbar(std::move(other.scrollbar)),
scrollSensitivity(std::move(other.scrollSensitivity)),
display(std::move(other.display)),
background(std::move(other.background)),
displayBuffer(std::move(other.displayBuffer)),
inputRequests(std::move(other.inputRequests)),
focused(std::move(other.focused)),
takeInput(std::move(other.takeInput)),
escapeNext(std::move(other.escapeNext)),
cursorBlink(std::move(other.cursorBlink)),
shell(std::move(other.shell)),
windowSetCursorDefault(std::move(other.windowSetCursorDefault)),
windowSetCursorIBeam(std::move(other.windowSetCursorIBeam)),
hovering(std::move(other.hovering)),
dragging(std::move(other.dragging)),
dragX(std::move(other.dragX)),
dragY(std::move(other.dragY)),
framebufValid(std::move(other.framebufValid)),
isInit(std::move(other.isInit))
{
    finishMove(other);
}
etm::Terminal &etm::Terminal::operator=(Terminal &&other) {
    errorCallback = std::move(other.errorCallback);
    resources = std::move(other.resources);
    viewport = std::move(other.viewport);
    scroll = std::move(other.scroll);
    scrollbar = std::move(other.scrollbar);
    scrollSensitivity = std::move(other.scrollSensitivity);
    display = std::move(other.display);
    background = std::move(other.background);
    displayBuffer = std::move(other.displayBuffer);
    inputRequests = std::move(other.inputRequests);
    focused = std::move(other.focused);
    takeInput = std::move(other.takeInput);
    escapeNext = std::move(other.escapeNext);
    cursorBlink = std::move(other.cursorBlink);
    shell = std::move(other.shell);
    windowSetCursorDefault = std::move(other.windowSetCursorDefault);
    windowSetCursorIBeam = std::move(other.windowSetCursorIBeam);
    hovering = std::move(other.hovering);
    dragging = std::move(other.dragging);
    dragX = std::move(other.dragX);
    dragY = std::move(other.dragY);
    windowSetCursorIBeam = std::move(other.windowSetCursorIBeam);
    framebufValid = std::move(other.framebufValid);
    isInit = std::move(other.isInit);

    finishMove(other);

    return *this;
}

void etm::Terminal::finishMove(Terminal &other) {
    other.resources = nullptr;
    display.setScroll(scroll);
    scrollbar.setScroll(scroll);
}

void etm::Terminal::init() {
    resources->init();
    isInit = true;
    updatePosition();
}

void etm::Terminal::deInit() {
    resources->deInit();
    isInit = false;
}

void etm::Terminal::setFont(const std::shared_ptr<EtmFont> &font) {
    resources->setFont(font);
    scroll.setAlign(resources->getFont()->getCharHeight());
    updatePosition();
}

void etm::Terminal::invalidate() {
    framebufValid = false;
}

void etm::Terminal::validate() {
    framebufValid = true;
}

void etm::Terminal::notifyScroll() {
    scrollbar.update();
    invalidate();
}

void etm::Terminal::initTex() {
    if (isInit) {
        resources->initTermTex(viewport.width, viewport.height);
    }
}

bool etm::Terminal::ignoreCodepoint(const Line::codepoint &c) {
    // Basically if it's a carrige return, we don't want it.
    return c == '\r';
}

void etm::Terminal::prepareInput() {
    // Cursor to the end of the text
    display.jumpCursor();
    // Lock the cursor's position so that it can't move backwards
    display.lockCursor();
    // Show it
    display.setCursorEnabled(true);
}

bool etm::Terminal::acceptInput() {
    return takeInput || inputRequests.size();
}


/// 0------------- streambuf overrides -----------------0

int etm::Terminal::sync() {
    flush();
    return 0;
}
std::streamsize etm::Terminal::showmanyc() {
    return displayBuffer.size();
}
std::streamsize etm::Terminal::xsgetn(char *out, std::streamsize size) {
    size = std::min(size, static_cast<std::streamsize>(displayBuffer.size()));
    displayBuffer.copy(out, size);
    displayBuffer.erase(0, size);
    return size;
}
int etm::Terminal::underflow() {
    if (displayBuffer.size()) {
        return displayBuffer.back();
    } else {
        return std::char_traits<char>::eof();
    }
}
int etm::Terminal::uflow() {
    if (displayBuffer.size()) {
        char c = displayBuffer.back();
        displayBuffer.pop_back();
        return c;
    } else {
        return std::char_traits<char>::eof();
    }
}
int etm::Terminal::pbackfail(int c) {
    if (std::char_traits<char>::eof() != c) {
        displayBuffer.insert(displayBuffer.begin(), c);
    }
    return c;
}
std::streamsize etm::Terminal::xsputn(const char *s, std::streamsize n) {
    displayBuffer.reserve(displayBuffer.size() + n);
    for (std::streamsize i = 0; i < n; i++) {
        displayBuffer.push_back(s[i]);
        if (s[i] == '\n') {
            flush();
        }
    }
    return n;
}
int etm::Terminal::overflow(int c) {
    if (std::char_traits<char>::eof() != c) {
        displayBuffer.push_back(c);
        if (c == '\n') {
            flush();
        }
    }
    return c;
}

/// 0------------- end streambuf overrides -----------------0

void etm::Terminal::pushInput(const std::string &input) {
    display.setCursorEnabled(false);

    // For convineince
    typedef std::string::const_iterator iterator_t;

    std::string filtered;
    filtered.reserve(input.size());
    bool escaped = false;
    for (iterator_t it = input.begin(); it < input.end(); ++it) {
        // Don't need to check for codepoint headers
        // because a value of a header can only ever be < 128
        // if it's a one-length ASCII char, and we only ever
        // check ASCII chars.
        if (escaped) {
            escaped = false;
            if (*it == '\n') {
                filtered.push_back(' ');
            } else {
                filtered.push_back(*it);
            }
        } else if (*it == '\\') {
            escaped = true;
        } else {
            filtered.push_back(*it);
        }
    }

    if (inputRequests.size()) {
        inputRequests.front()->terminalInput(filtered);
        inputRequests.pop_front();
    } else if (shell != nullptr) {
        shell->input(filtered);
    } else {
        resources->postError(
            "Terminal::flushInputBuffer()",
            "Shell is nullptr (not set)",
            0,
            false
        );
    }
    // Only re-enable the cursor if there are still input requests
    if (acceptInput()) {
        prepareInput();
    }
}

void etm::Terminal::postError(const termError &error) {
    errorCallback(error);
}

void etm::Terminal::setCursorDefault(const winActionCB_t &callback) {
    windowSetCursorDefault = callback;
}
void etm::Terminal::setCursorIBeam(const winActionCB_t &callback) {
    windowSetCursorIBeam = callback;
}
void etm::Terminal::setHovering(bool value) {
    if (hovering != value) {
        hovering = value;
        if (hovering) {
            if (windowSetCursorIBeam) {
                windowSetCursorIBeam();
            }
        } else if (windowSetCursorDefault) {
            windowSetCursorDefault();
        }
    }
}
void etm::Terminal::setErrorCallback(const errCallback_t &callback) {
    if (callback) {
        errorCallback = callback;
    }
}

void etm::Terminal::clearInput() {
    display.clearInput();
    display.jumpCursor();
}

void etm::Terminal::clear() {
    display.clear();
    displayBuffer.clear();
    displayBuffer.shrink_to_fit();
}

void etm::Terminal::setBackgroundColor(const Color &color) {
    background.setColor(color);
    display.setDefBackGColor(color);
}
void etm::Terminal::setTextColor(const Color &color) {
    display.setDefForeGColor(color);
}

void etm::Terminal::setScrollSensitivity(float value) {
    scrollSensitivity = value;
}

void etm::Terminal::setScrollCooldown(int millis) {
    scrollbar.setScrollCooldown(millis);
}
void etm::Terminal::setScrollWait(int millis) {
    scrollbar.setScrollWait(millis);
}

// The shell is where user input will be directed
void etm::Terminal::setShell(EShell &shell) {
    this->shell = &shell;
}

void etm::Terminal::setTakeInput(bool value) {
    if (takeInput != value) {
        if (!acceptInput()) {
            prepareInput();
        }
        takeInput = value;
    }
}

void etm::Terminal::requestInput(TermInput &callback) {
    if (!acceptInput()) {
        prepareInput();
    }
    inputRequests.push_back(&callback);
}
void etm::Terminal::cancelInputRequest(TermInput *callback) {
    typedef inputRequests_t::iterator it_t;
    for (it_t it = inputRequests.begin(); it < inputRequests.end();) {
        if (*it == callback) {
            it = inputRequests.erase(it);
        } else {
            ++it;
        }
    }
    if (!acceptInput()) {
        display.setCursorEnabled(false);
    }
}
void etm::Terminal::clearInputRequests() {
    inputRequests.clear();
    if (!takeInput) {
        display.setCursorEnabled(false);
    }
}

std::string etm::Terminal::pollInput() {
    return display.pollInput();
}

void etm::Terminal::dispText(const std::string &str) {
    displayBuffer += str;
}

int etm::Terminal::readHexFromStr(const std::string &str, std::string::size_type &i) {
    i++;
    int result = 0;
    for (std::string::size_type end = i + 6; i < str.size() && i < end && str[i] != ';'; i++) {
        if ('0' <= str[i] && str[i] <= '9') {
            // Shift one hex diget to the left for every new diget we find
            result = (result * 16) + str[i] - '0';
        } else {
            // Sets to lowercase (kinda' a hack)
            const char c = str[i] | 0x20;
            if ('a' <= c && c <= 'f') {
                result = (result * 16) + c - 'a' + 10;
            } else {
                break;
            }
        }
    }
    // Decriment so that is on the last char in the hex
    i--;
    return result;
}

void etm::Terminal::flush() {
    softFlush();

    // Disrupt the cursor
    display.jumpCursor();
    display.lockCursor();
}

void etm::Terminal::softFlush() {
    constexpr char ESCAPE = '\x1b';

    // Required when appending
    display.prepare();

    for (std::string::size_type i = 0; i < displayBuffer.size();) {
        // Escape char
                                        // 1 for the [, 1 for the spec (b/f)
        if (displayBuffer[i] == ESCAPE && i + 2 < displayBuffer.size() && displayBuffer[i+1] == '[') {
            std::string::size_type fi = i + 2;
            std::shared_ptr<tm::Mod> mod;
            switch (displayBuffer[fi]) {
                case 'b':
                    mod = std::make_shared<tm::Background>(readHexFromStr(displayBuffer, fi));
                    break;
                case 'f':
                    mod = std::make_shared<tm::Foreground>(readHexFromStr(displayBuffer, fi));
                    break;
                case 'B':
                    mod = std::make_shared<tm::RevBackground>();
                    break;
                case 'F':
                    mod = std::make_shared<tm::RevForeground>();
                    break;
                case 'r':
                    mod = std::make_shared<tm::Revert>();
                    break;
            }
            if (mod) {
                i = fi+1;
                // Skip the optional semicolon
                if (i < displayBuffer.size() && displayBuffer[i] == ';') {
                    i++;
                }
                display.pushMod(mod);
                continue;
            }
        }
        const int size = utf8::test(displayBuffer[i]);
        if (i + size <= displayBuffer.size()) {
            Line::codepoint c(displayBuffer.begin() + i, displayBuffer.begin() + (i + size));
            display.append(c);
        }
        i += size;
    }

    displayBuffer.clear();

    // Jump to end if the user is scrolled to the end
    const bool jump = scroll.getOffset() + 1 >= scroll.getMaxOffset();
    scroll.setGrossHeight(display.getHeight());
    if (jump) {
        scroll.jump();
    }
    scrollbar.update();

    invalidate();
}

void etm::Terminal::setX(float x) {
    viewport.x = x;
    updatePosition();
}
void etm::Terminal::setY(float y) {
    viewport.y = y;
    updatePosition();
}

void etm::Terminal::setWidth(int width) {
    viewport.width = width;
    updatePosition();
}
void etm::Terminal::setHeight(int height) {
    viewport.height = height;
    updatePosition();
}
void etm::Terminal::setColumns(int columns) {
    viewport.width = resources->getFont()->getCharWidth() * columns;
    updatePosition();
}
void etm::Terminal::setRows(int rows, int margin) {
    viewport.height = resources->getFont()->getCharHeight() * rows;
    viewport.height += margin;
    updatePosition();
}

void etm::Terminal::setFontSize(unsigned int size) {
    resources->getFont()->setSize(size);
    // Align to char height
    scroll.setAlign(resources->getFont()->getCharHeight());
    updatePosition();
}

void etm::Terminal::setMaxLines(TextBuffer::lines_number_t count) {
    display.setMaxLines(count);
}

void etm::Terminal::updatePosition() {
    background.setX(0);
    background.setY(0);
    background.setWidth(viewport.width - scrollbar.getWidth());
    background.setHeight(viewport.height);

    scrollbar.setX(0 + viewport.width - scrollbar.getWidth());
    scrollbar.setY(0);
    scrollbar.setHeight(viewport.height);

    if (resources->getFont()) {
        // Align net height to the character height
        scroll.setNetHeight(viewport.height - (static_cast<int>(viewport.height) % resources->getFont()->getCharHeight()));

        // Takes number of columns
        display.setWidth(background.getWidth() / resources->getFont()->getCharWidth());
    }

    initTex();

    invalidate();
}

void etm::Terminal::inputChar(unsigned int codepoint) {
    std::string encoded = utf8::encode(codepoint);
    Line::codepoint c(encoded.cbegin(), encoded.cend());
    inputChar(c);
}
void etm::Terminal::inputChar(const Line::codepoint &c) {
    if (acceptInput()) {
        doInputChar(c);
        // When inputting, scroll to focus on the input
        // area, which is always at the bottom (or should be...)
        scroll.setGrossHeight(display.getHeight());
        scroll.jump();
        scrollbar.update();
        invalidate();
    }
}
void etm::Terminal::doInputChar(const Line::codepoint &c) {

    if (!ignoreCodepoint(c)) {
        if (!escapeNext) {
            switch (*c.start) {
                case '\n': {
                    // Don't actually include the newline
                    // in the input
                    std::string input = display.pollInput();
                    display.append(c);
                    pushInput(input);
                    break;
                }
                case '\\': // Fallthrough
                    escapeNext = true;
                default:
                    display.insertAtCursor(c);
            }
        } else {
            escapeNext = false;
            display.insertAtCursor(c);
        }
        invalidate();
    }
}

void etm::Terminal::inputString(const std::string &text) {
    for (std::string::const_iterator it = text.begin(); it < text.end();) {
        if (!acceptInput()) break;
        const int size = utf8::test(*it);
        // Don't trust the offset given
        if (it + size <= text.end()) {
            Line::codepoint c(it, it + size);
            inputChar(c);
        }
        it += size;
    }
}
void etm::Terminal::inputActionKey(actionKey key) {
    // Parts of the function depend on a valid shell
    if (shell == nullptr) {
        resources->postError(
            "Terminal::inputActionKey(actionKey)",
            "Shell is nullptr (not set)",
            0,
            false
        );
        return;
    }
    switch (key) {
        case ENTER:
            inputChar('\n');
            break;
        case BACKSPACE:
            display.eraseAtCursor();
            invalidate();
            break;
        case UP:
            if (!display.moveCursorRow(-1)) {
                shell->cursorUp();
            }
            break;
        case DOWN:
            if (!display.moveCursorRow(1)) {
                shell->cursorDown();
            }
            break;
        case LEFT:
            display.moveCursorCollumnWrap(-1);
            break;
        case RIGHT:
            display.moveCursorCollumnWrap(1);
            break;
        // Ignore if there's no match
    }
}
void etm::Terminal::inputMouseScroll(float yOffset, float mouseX, float mouseY) {
    if (viewport.hasPoint(mouseX, mouseY)) {
        // Negate to align properly
        scroll.scroll(-yOffset * scrollSensitivity);
    }
}
void etm::Terminal::inputMouseClick(bool isPressed, float mouseX, float mouseY) {
    mouseX -= viewport.x;
    mouseY -= viewport.y;
    scrollbar.mouseClick(isPressed, mouseX, mouseY);
    if (isPressed) {
        focused = background.hasPoint(mouseX, mouseY);
        dragging = focused;

        if (dragging) {
            // Text highlighting
            TextBuffer::lines_number_t row;
            TextBuffer::line_index_t column;
            mapCoords(mouseX, mouseY, row, column);
            display.initSelection(row, column);
            invalidate();
        }
    } else {
        dragging = false;
    }
}
void etm::Terminal::inputMouseMove(float mouseX, float mouseY) {
    mouseX -= viewport.x;
    mouseY -= viewport.y;
    scrollbar.mouseMove(mouseX, mouseY);
    setHovering(background.hasPoint(mouseX, mouseY));
    if (dragging) {
        // Text highlighting
        TextBuffer::lines_number_t row;
        TextBuffer::line_index_t column;
        mapCoords(mouseX, mouseY, row, column);
        display.setSelectionEnd(row, column);
        invalidate();
    }
}

void etm::Terminal::mapCoords(float x, float y, TextBuffer::lines_number_t &row, TextBuffer::line_index_t &column) {
    row = std::max(std::floor((y + scroll.getOffset()) / resources->getFont()->getCharHeight()), 0.0f);
    column = std::max(std::floor(x / resources->getFont()->getCharWidth()), 0.0f);
}

void etm::Terminal::render() {
    // Preserve caller state (important!)
    State state;
    state.set();

    resources->setTerminal(*this);

    // Run animiations

    if (cursorBlink.hasEnded()) {
        display.toggleCursor();
        cursorBlink.start();
    }

    scrollbar.animate();

    // Render

    if (!framebufValid) {
        Framebuffer::State fbState;
        // None of the called functions in this
        // block should throw exceptions
        GLint callerViewport[4];
        glGetIntegerv(GL_VIEWPORT, callerViewport);

        resources->bindTermFramebuffer();

        glViewport(0, 0, viewport.width, viewport.height);
        resources->initViewport();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render

        resources->bindPrimitiveShader();
        background.render();
        scrollbar.render();
        // After scrollbar renders, the text shader is set
        // due to the rendering of the triangle textures,
        // so display doesn't need to set it.
        display.render();

        // Revalidate cache (important!)
        validate();

        glViewport(callerViewport[0], callerViewport[1], callerViewport[2], callerViewport[3]);
    }

    // We won't be changing the viewport
    resources->initViewport();

    resources->bindTextureShader();
    resources->bindTermFramebufferTex();
    viewport.set(resources);
    resources->renderRectangle();

    // Redering the cursor every time isn't a big deal,
    // and we have to isolate it anyways because it's
    // an animation
    display.renderCursor(viewport.x, viewport.y);

    // Check for errors

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        resources->postError(
            "Terminal::render()",
            std::string("OpenGL error - ") + getGLErrorStr(error),
            error,
            true
        );
    }
}

bool etm::Terminal::isFocused() {
    return focused;
}

void etm::Terminal::setFocused(bool val) {
    focused = val;
}

std::string etm::Terminal::getTextSelection() {
    return display.getSelectionText();
}

std::string etm::Terminal::getText() {
    return display.getTextFromRange(
        TextBuffer::pos(0, 0),
        TextBuffer::pos(display.getCountRows() + 1, 0)
    );
}
