#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

#include <string>

#include "../render/Font.h"
#include "../render/Color.h"
#include "Image.h"

namespace etm { class Resources; }

/*
* Text, also called "Static Text", is a static text widget that displays text.
* To actually load the given string into the bitmap, you must call generate(),
* which will render the string based on the given wrapping width, line spacing mod, etc.
* The text can be set a "overflow height", a height at which the lines will be truncated to
* and pushed to the side. Those extra lines are in groups I call "overflows", and can be
* rendered in peices through calls to nextOverflow() and related functions.
* If you need text that is progressively rendered, see TextCrawl.
*/

namespace etm {
    class Text { 
    protected:
        // The in-use font
        Resources *res;

        // The image handle, renders the generated bitmap
        Image image;

        // The color of the text
        Color color;

        // The string to render
        std::string str;
        // The max width (inclusive) of the string.
        // zero indicates that line wrapping should be disabled.
        Font::map_size wrappingWidth;
        // After what height the text should be truncated and pushed
        // to the next set.
        // Render the next set by calling nextOverflow(), etc.  
        Font::map_size overflowHeight;
        // Modifier for the distance between lines.
        // 1 is the font maker's "preferred" spacing,
        // 0 should make the text render on top of itself
        float lineSpacingMod;
        // The text align of the text
        Font::textAlign textAlign;
        // The pixel size that the text should be rendered as
        unsigned int fontSize;

        // The index'd string, represented as glyph indices
        Font::glyph_str index;
        // The computed lines
        Font::line_str lines;
        // The current stop
        std::vector<Font::line_iterator>::size_type stopsIndex;
        // A series of stops, representing different locations in the overflow
        std::vector<Font::line_iterator> stops;

        // Render lines to image
        void loadLines(const Font::line_iterator &start, const Font::line_iterator &end);
        // Calls loadLines(...) with the line range for the current stopsIndex.
        // Everything from the iterator at stopsIndex-1 and stopsIndex are rendered.
        // if stopsIndex is zero, starts rendering from the beginning of `lines`
        void loadStops();

        void textSteal(Text &other);
        void textCopy(const Text &other);
        // Configures the font so that it renders according
        // to this object's settings. Should be done before most every function call.
        // ATM all this does is set the font size
        void configureFont();
        // Painstakingly copies iterator data because APPARENTLY
        // moving vectors _might_ invalidate iterators.
        // Well, that's quite a might, but just to be safe...
        // https://stackoverflow.com/a/11022447/10821333
        inline void textCopyIL(const Text &other);
        inline void textCopyPOD(const Text &other);
    public:
        // Default construct.
        // Will be ill-formed!!!!!!!!!!!!!!!!!!!!!1
        Text();
        Text(Resources *res);
        Text(const Text &other);
        Text(Text &&other);
        Text &operator=(const Text &other);
        Text &operator=(Text &&other);

        // Clear the text and have it display a blank.
        // Similar effect is achieved with empty string or string with only spaces and newlines
        // virtual because TextCrawl will have different methods for doing this, since it renders things
        // dynamically
        void clear();

        // x/y pos of text
        int getX();
        int getY();
        // String to render.
        // Is reference return here is a bad idea?
        std::string &getString();
        // Text align
        Font::textAlign getAlign();

        // Set previous values
        void setX(int x);
        void setY(int y);
        void setString(const std::string &str);
        void setAlign(Font::textAlign textAlign);

        // Get reference to color object.
        // Represents the color of the text
        Color getColor();
        void setColor(const Color &color);

        // The width at which the text will be wrapped, exclusive.
        // A value of 0 disables wrapping.
        // If it is disabled, overflow height is also implicitly disabled,
        // because the line height is constant
        void setWrappingWidth(unsigned int width);
        // The height at which the text will be truncated, exclusive.
        // A value of 0 disables truncation.
        // No overflows will be generated if overflow height is disabled.
        void setOverflowHeight(unsigned int height);
        // Modifier for the distance between lines.
        // 1 is the font maker's "preferred" spacing,
        // 0 should make the text render on top of itself
        void setLineSpacingMod(float mod);
        unsigned int getWrappingWidth();
        unsigned int getOverflowHeight();
        float getLineSpacingMod();

        // The pixel size of the font
        void setFontSize(unsigned int size);
        unsigned int getFontSize();

        // The width and height of the current texture
        unsigned int getWidth();
        unsigned int getHeight();

        // Render the next overflow segment, overwriting the current texture.
        // Returns false if reached the end of the overflows.
        bool nextOverflow();
        // The number of overflow segments.
        // Includes the non-overflowing start.
        Font::line_str::size_type overflowSize();
        // Render a specific overflow section
        void seekOverflow(Font::line_str::size_type i);

        // Generates the required render metadata for the text rendering
        // based off of the font size, overflow height, wrapping width, line
        // spacing mod, string and text align.
        // It then calls the render function loadLines(...)
        void generate();
        // Renders the text
        void render();
    };
}

#endif
