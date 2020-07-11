#include "Text.h"

#include <iostream>
#include <cmath>

#include "../render/glfw.h"
#include "../Resources.h"

etm::Text::Text():
    res(nullptr),
    image(nullptr),
    wrappingWidth(0), overflowHeight(0),
    lineSpacingMod(1.0f), textAlign(etm::Font::textAlign::left),
    fontSize(etm::Font::defaultSize), stopsIndex(0)
{
}

etm::Text::Text(Resources *res):
    res(res),
    image(res),
    wrappingWidth(0), overflowHeight(0),
    lineSpacingMod(1.0f), textAlign(etm::Font::textAlign::left),
    fontSize(etm::Font::defaultSize), stopsIndex(0)
{
}

// Memory manegemet

void etm::Text::textSteal(Text &other) {
    image = std::move(other.image);
    color = std::move(other.color);

    str = std::move(other.str);
    stops = std::move(other.stops);

    textCopyIL(other);
    textCopyPOD(other);
}

void etm::Text::textCopy(const Text &other) {
    image = other.image;
    color = other.color;

    str = other.str;
    stops = other.stops;

    textCopyIL(other);
    textCopyPOD(other);
}

void etm::Text::textCopyIL(const Text &other) {
    // https://stackoverflow.com/questions/11021764/does-moving-a-vector-invalidate-iterators
    // So iterators _might_ be invalidated...
    // I saw another post saying that they aren't as long as
    // the allocaters are the same or something, but I'm just
    // going to stay on the safe side.
    index = other.index;
    lines = other.lines;
    for (etm::Font::line &ln : lines) {
        ln.start = index.begin() + (ln.start - other.index.begin());
        ln.end = index.begin() + (ln.end - other.index.begin());
    }
    for (etm::Font::line_iterator &it : stops) {
        it = lines.begin() + (it - other.lines.begin());
    }
}

void etm::Text::textCopyPOD(const Text &other) {
    res = other.res;
    wrappingWidth = other.wrappingWidth;
    std::cout << "other.wrappingWidth = " << other.wrappingWidth << std::endl;
    overflowHeight = other.overflowHeight;
    lineSpacingMod = other.lineSpacingMod;
    textAlign = other.textAlign;
    fontSize = other.fontSize;
    stopsIndex = other.stopsIndex;
}

etm::Text::Text(const Text &other): image(nullptr) {
    textCopy(other);
    // Only render the bimap if there are lines to render
    // Old code for compatability with TextCrawl,
    // which isn't used here.
    // if (lines.size()) {
    //     loadStops();
    // }
}

etm::Text::Text(Text &&other): image(nullptr) {
    textSteal(other);
}

etm::Text &etm::Text::operator=(const Text &other) {
    textCopy(other);
    // if (lines.size()) {
    //     loadStops();
    // }
    return *this;
}

etm::Text &etm::Text::operator=(Text &&other) {
    textSteal(other);
    return *this;
}

// End memory manegemet

void etm::Text::configureFont() {
    res->getFont().setSize(fontSize);
}

void etm::Text::clear() {
    // Clear the image of all data,
    // and wipe the lines so that no mistakes are
    // made
    image.setImage(GL_RGBA, 0, 0, nullptr);
    image.setWidth(0);
    image.setHeight(0);
    lines.clear();
}

void etm::Text::generate() {
    if (!str.size()) {
        clear();
        return;
    }
    // Determine if the string contains only whitespace.
    // If so, a clear() would have the same effect,
    // and would prevent a crash fron allocating a zero
    // length array later on in Font.
    for (std::string::size_type i = 0;; i++) {
        if (i >= str.size()) {
            clear();
            return;
        }
        if (str[i] != ' ' && str[i] != '\n') {
            break;
        }
    }

    // Setup font for rendering operations
    configureFont();
    index = res->getFont().indexString(str.cbegin(), str.cend());
    if (wrappingWidth) {
        std::cout << "wrap " << wrappingWidth << std::endl;
        lines = res->getFont().getLines(index.cbegin(), index.cend(), wrappingWidth, textAlign);
        if (overflowHeight) {
            stops = res->getFont().breakupLines(lines.cbegin(), lines.cend(), lineSpacingMod, overflowHeight);
        } else {
            stops.clear();
            stops.push_back(lines.cend());
        }
    } else {
        // If wrapping width has been turned off (set to zero), then
        // make a dummy line from the entire index.
        lines.clear();
        lines.push_back(res->getFont().makeLine(index.cbegin(), index.cend()));
        // If wrapping width is disabled, then overflow height is
        // irrelevant because the line height is constant.
        stops.clear();
        stops.push_back(lines.cend());
    }

    // Render lines up until the first stop
    loadLines(lines.cbegin(), stops[0]);
}

void etm::Text::loadLines(const etm::Font::line_iterator &start, const etm::Font::line_iterator &end) {
    configureFont();
    unsigned int width, height;
    unsigned char *bitmap = res->getFont().renderLines(start, end, 4, color.get(), lineSpacingMod, width, height);
    image.setWidth(width);
    image.setHeight(height);
    image.setImage(GL_RGBA, width, height, bitmap);

    delete[] bitmap;
}

void etm::Text::loadStops() {
    etm::Font::line_iterator start;
    if (stopsIndex != 0) {
        start = stops[stopsIndex - 1];
    } else {
        start = lines.begin();
    }
    loadLines(start, stops[stopsIndex]);
}

void etm::Text::render() {
    image.render();
}

bool etm::Text::nextOverflow() {
    if (stopsIndex + 1 < overflowSize()) {
        stopsIndex++;
        loadStops();
        return true;
    }
    return false;
}

etm::Font::line_str::size_type etm::Text::overflowSize() {
    return stops.size();
}

void etm::Text::seekOverflow(etm::Font::line_str::size_type i) {
    if (i < overflowSize()) {
        stopsIndex = i;
        loadStops();
    }
}

/* Setter bloat */


etm::Color etm::Text::getColor() {
    return color;
}

void etm::Text::setColor(const Color &color) {
    this->color = color;
}

int etm::Text::getX() {
    return image.getX();
}

int etm::Text::getY() {
    return image.getY();
}

std::string &etm::Text::getString() {
    return str;
}

etm::Font::textAlign etm::Text::getAlign() {
    return textAlign;
}

void etm::Text::setX(int x) {
    image.setX(x);
}

void etm::Text::setY(int y) {
    image.setY(y);
}

void etm::Text::setString(const std::string &str) {
    this->str = str;
}

void etm::Text::setAlign(etm::Font::textAlign textAlign) {
    this->textAlign = textAlign;
}


void etm::Text::setWrappingWidth(unsigned int width) {
    std::cout << "das" << std::endl;
    wrappingWidth = width;
}

void etm::Text::setOverflowHeight(unsigned int height) {
    overflowHeight = height;
}

void etm::Text::setLineSpacingMod(float mod) {
    lineSpacingMod = mod;
}

unsigned int etm::Text::getWrappingWidth() {
    return wrappingWidth;
}

float etm::Text::getLineSpacingMod() {
    return lineSpacingMod;
}

void etm::Text::setFontSize(unsigned int size) {
    fontSize = size;
}
unsigned int etm::Text::getFontSize() {
    return fontSize;
}

unsigned int etm::Text::getWidth() {
    return image.getWidth();
}

unsigned int etm::Text::getHeight() {
    return image.getHeight();
}
