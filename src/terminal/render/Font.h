#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include <string>
#include <vector>
#include <exception>

// The freetype face handle
// The original typedef is
// typedef struct FT_FaceRec_*  FT_Face;
// If FT_Face isn't typedef'd as such, we have
// a problem.
// Will be checked by static assert in Font.cpp
struct FT_FaceRec_;
// Same deal
struct FT_LibraryRec_;

/*
* Font's, aka faces, are instances of a font that are used to render text.
* Font size is a global property, and should be set before any rendering operation.
* The font itself can be set to render in different configurations. Those can be
* done manually by calling getFace() and using Freetype functions.
* The Freetype docs can be found here https://www.freetype.org/freetype2/docs/reference/
* To clear up some jargon:
* "glyph" here is more or less the font's UID for a given character.
* "26.6" referrs to 26.6 fractional pixels/integers. It's where the last 6 bits of an integer
* value represent a fractional value. So, for example, an integer 00000000 00000000 00011011 00111100
* is actually 108.9375 instead of 6972.
* "pen", or the "drawing pen", is the x/y origin where a given glyph is rendered. This is more
* just a point of reference for Freetype than anything, because as can be seen the first diagram
* in this page, https://www.freetype.org/freetype2/docs/tutorial/step2.html , the origin is hardly
* the minX/maxY of the glyph. Also worthy of note: at the time of writing, pen position is recorded
* in 26.6 fractional pixels, and is rounded to a 32 bit integer when rendering.
* "lsb", or left side bearing, is the horizontal distance from the origin to the leftmost
* pixel in the bitmap.
*/

namespace etm {
    class Font {
    public:
        // I just don't want to include the freetype
        // header, alright?
        // Took typedefs from docs.
        // If they change, I'll know due to static assertion
        // inside Font.cpp
        typedef signed long FT_Pos_for;
        typedef int FT_Error_for;
        typedef FT_FaceRec_ *FT_Face_for;
        typedef FT_LibraryRec_ *FT_Library_for;

        // Type that glyphs are represented as.
        typedef unsigned int glyph_type;
        // A sequence of glyphs
        typedef std::vector<glyph_type> glyph_str;
        // An iterator for a glyph sequence
        typedef glyph_str::const_iterator glyph_iterator;
        // Bitmap size type
        typedef unsigned int map_size;
        // Bitmap offset type
        typedef int ofs_type;

        // A sigle line, directs what characters of
        // a glyph_str to draw, and contains other metadata
        // about the line.
        struct line {
            // The iterators describe the range
            // of glyphs to render.
            // Note that, because these are iterators,
            // the source container must not be deallocated,
            // or modified to where iterators become invalidated.
            // This is very important to remember.
            // Perhaps that's something that should be changed?
            // I originally did it this way because I didn't want to
            // have to bother copying a bunch of ranges, and because
            // I fancied it more memory efficient.
            // Well regardless, it's easy to change if it becomes too big
            // of an issue.

            // The start iterator, inclusive
            glyph_iterator start;
            // The stop iterator, exlcusive
            glyph_iterator end;

            // These two are in 26.6 fractional pixels,
            // in part for efficiency and also for accuracy
        
            // The x-shift of the line.
            // This is important for characters like "j"
            // which extends its jaw behind where the pen is.
            ofs_type horizontalOffset; // 26.6 fractional
            // The total width of the line
            map_size width; // 26.6 fractional
            line(); // zero out
            line( // With values
                const glyph_iterator &start, const glyph_iterator &end,
                ofs_type horizontalOffset, map_size width);
        };

        // A sequence of lines
        typedef std::vector<line> line_str;
        // An iterator for a sequence of lines
        typedef line_str::const_iterator line_iterator;

        // A set of configurations for rendering
        // characters into bitmaps
        struct renderConf {
            // The number of desired channels
            // ex RGB = 3, RGBA  = 4, etc.
            int channels;
            // Modifiers to each channel value
            // typically these will be the RGBA values
            // represented 0-1.
            // Must be at least long as `channels`, else undefined
            // behavior is invoked.
            float *cMod;
            // The output bitmap
            unsigned char *bitmap;
            // The distance in pixels that must be travelled across the bitmap,
            // that is, the bytes that must be travelled divided by the number of channels
            // Often times this is the pixel width of the bitmap
            unsigned int verticalStride;
            // Vertical distance travelled per line
            map_size lineHeight;

            // Zero initialize
            renderConf();
            // Construct with values
            renderConf(const int channels, float *cMod, unsigned char *bitmap, unsigned int verticalStride, map_size lineHeight);
        };

        // Holds an x/y pair
        // At the moment only used for the pen's position
        struct vec2 {
            map_size x;
            map_size y;

            // Zero initialize
            vec2();
            // Construct with values
            vec2(map_size x, map_size y);
        };

        // Conventional text align parameters,
        // these are given to getLines(...) to get line
        // render data attuned to a certain align
        enum textAlign {
            left,
            right,
            center
        };

        class glyph_render_fail: public std::exception {
            std::string str;
        public:
            glyph_render_fail(int code) noexcept;
            const char *what() const noexcept override;
        };

    private:
        // Pointer to Freetype face (aka font) object.
        // Yes, FT_Face is typedef'd as `struct FT_FaceRec_*`
        // https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_face
        FT_Face_for face;
        // This font's glyph representation of a space.
        // Important for things such as line wrap, where spaces and
        // other whitespace can be broken on at no consequence, whereas
        // actual words cannot and must be preserved
        glyph_type space;
        // Switch between the newline defined by the font and our own value,
        // because many fonts will just have newline as a nul (0)
        glyph_type fontNewline;
        // Metadata for a specific glyph
        // That name's off...
        struct char_metrics {
            // Left side bearing, the space to the left of the pen
            // that the character is rendered to
            int lsb;
            // Additional offset applied to the pen to account for
            // character combinations like AV, where they are scrunched
            // together in order to remove extra space in-bewteen.
            // Only done for a few characters.
            int kern;
            // The x distance that the pen must be moved after this glyph has been rendered
            // type -> https://www.freetype.org/freetype2/docs/reference/ft2-basic_types.html#ft_pos
            FT_Pos_for advance;
        };
        // Checks if a glyph is that of the newline control value.
        // If it is, it returns `fontNewline`. Else, it returns `glyph`.
        // This is needed because fonts normally represent a newline as a nul
        // character, and we need to know what glyph are newlines in the getLines(...)
        // function, for they signify a line break.
        // Therefore, we have a control value, which is the max value of an unsigned integer.
        // This converts our glyphs back to the values defined by the font.
        inline glyph_type getGlyph(glyph_type glyph);
        void steal(Font &other);
        // Copy Plain Old Data - stuff that can't really be moved.
        void copyPOD(const Font &other);
        // Delete the wrapped face and reset the wrapped shared pointer
        void free();
        // Get char_metrics from a given glyph range
        std::vector<char_metrics> measureString(glyph_iterator start, const glyph_iterator &end);
    public:
        // The size that all fonts will start off at
        static constexpr unsigned int defaultSize = 16;
        // Construct with required data
        // `face` is a Freetype face (aka font), `data` is a shared pointner
        // to the font data.
        // Font should be a true type (.tff) font.
        // Throws an instance of `err::ft_error` if failed to load the font
        Font(FT_Library_for lib, const std::string &fontPath);
        Font(Font &&other);
        ~Font();
        Font &operator=(Font &&other);


        /* start public methods */

        // Set the character pixel size.
        // After this is set, it's set for all functions.
        // Returns the error status of the operation, a
        // Freetype error code.
        // As of writing, Freetype says that FT_NO_ERROR is 0
        FT_Error_for setSize(unsigned int size);

        /* end public methods */

        /* private methods, use only if you know what you're doing */

        // Get the handle to the internal Freetype face
        FT_Face_for getFace();

        // Renders the given glyph range to `render`'s bitmap
        // Renders with origin at top-left; ie, increasing y is down
        // IMPORTANT: Assumes that the bitmap in `render` has already been allocated to
        // the necessary size.
        // `horizontalOffset` is the start x of the pen in 26.6 fractional pixels
        void renderTo(glyph_iterator start, const glyph_iterator &end, const renderConf &render, ofs_type horizontalOffset);
        // Renders a single glyph, `glyph`, to `render`'s bitmap based on the x/y values in `pen`
        // `next` is the next glyph, the one that the kerning should be based off of.
        // If there is no next glyph, use `0`.
        // `pen`'s values are assumed to be in 26.6 fractional pixels
        void renderGlyphTo(glyph_type glyph, glyph_type next, const renderConf &render, vec2 &pen);
        // Renders a glyph range as a single line.
        // Use if you're in a rush.
        // `channels` is the number of channels per pixel
        // `cMod` is an array of length >= `channels`. Each value in `cMod` is multiplied by
        // its corresponding pixel channel. This is similar to 3 channels -> RGB -> 1, 0, 0 -> red color.
        // Default value for each channel is 255 (max uchar value).
        // `width` and `height` will have the corresponding bitmap metadata assigned to them,
        // hence they're taken as references.
        // Returns an allocated bitmap containing the rendered glyphs.
        unsigned char *renderBitmap(const glyph_iterator &start, const glyph_iterator &end, const int channels, float cMod[], map_size &width, map_size &height);
        // Renders a range of lines.
        // `channels` is the number of channels per pixel
        // `cMod` is an array of length >= `channels`. Each value in `cMod` is multiplied by
        // its corresponding pixel channel. This is similar to 3 channels -> RGB -> 1, 0, 0 -> red color.
        // Default value for each channel is 255 (max uchar value).
        // `width` and `height` will have the corresponding bitmap metadata assigned to them,
        // hence they're taken as references.
        // `lineSpacingMod` is a multiplier for the line descent, which is the vertical distance
        // the pen travels from line to line. Setting it to zero will cause the lines to be drawn on top of
        // each-other, although I haven't tried it.
        unsigned char *renderLines(line_iterator start, const line_iterator &end, const int channels, float cMod[], float lineSpacingMod, map_size &width, map_size &height);
        // makes a single line from the given glyph range.
        // This is used to enable compatability where your code works entirely around lines.
        line makeLine(const glyph_iterator &start, const glyph_iterator &end);
        // Takes a given glyph range and extracts lines based on the given `wrappingWidth`
        // and the newlines contained in the glyph range.
        // Take note that the source glyph_str object must remain allocated and not be modifed so
        // as to invalidate iterators, because a Font::line contains iterators based off of the
        // given iterators.
        // The lines will have x offsets determined by glyph properties and the given `align`
        // `wrappingWidth` is later converted to 26.6 fractional pixels, so you cannot use values larger than 2^26-1...
        // However that shouldn't be a problem. There are a lot of other things that can break with very large numbers...
        line_str getLines(const glyph_iterator &start, const glyph_iterator &end, unsigned int wrappingWidth, const textAlign align);
        // Gets metadata for a given glyph range.
        // Any parameter as `nullptr` will be ignored, however note that there will be no measurable
        // improvements to performance by doing so; what I mean is, all the other value are still calculated.
        // NB: `horizontalOffset` will be in 26.6 fractional pixels
        void stringMetrics(glyph_iterator start, const glyph_iterator &end, map_size *width, map_size *height, ofs_type *horizontalOffset);
        // Gets the descent of the given range, ie, the max distance below the pen
        // that the glyphs will be rendered.
        // Return as 26.6
        int stringDescent(glyph_iterator start, const glyph_iterator &end);
        // Gets the ascent of the given range, ie, the max distance above the pen
        // that the glyphs will be rendered.
        // Return as 26.6
        // This value coupled with stringDescent(...) can represent the height of the glyph range.
        map_size stringAscent(glyph_iterator start, const glyph_iterator &end);
        // Convert character range to a glyph string for faster processing
        glyph_str indexString(std::string::const_iterator start, const std::string::const_iterator &end);
        // Breaks the given lines up into groups, each with a combined height less than or equal to `maxHeight` given
        // the provided `lineSpacingMod`.
        // Returns a vector if iterators representing the exclusive end of each line group.
        // The vector will have at least one element.
        // The line groups start at the parameter `start`.
        // Hence, you would render the lines as so:
        // 
        // std::vector<line_iterator> groups = breakupLines(start, ...);
        // 
        // typedef std::vector<line_iterator>::const_iterators group_iterator;
        // line_iterator last = start;
        // for (group_iterator g = groups.cbegin(); g < groups.cend(); ++g) {
        //     renderLines(last, *g, ...);
        //     last = *g;
        // }
        // 
        // Note to self: Should the returned vector include the start iterator? Does it work well? Is it good practice?
        // 
        std::vector<line_iterator> breakupLines(line_iterator start, const line_iterator &end, float lineSpacingMod, map_size maxHeight);
        // Truncates the lines to where lines from `start` inclusive to [the return value] exclusive are collectively
        // less than or equal to `maxHeight`.
        // Essentially does the same as breakupLines(...), except it only returns what would be the first
        // element in the vector returned by breakupLines(...), and as a result is more efficient.
        line_iterator truncLines(line_iterator start, const line_iterator &end, float lineSpacingMod, map_size maxHeight);
        // Returns the pixel height of the line range, given a `lineSpacingMod`.
        // `lineSpacingMod` is a multiplier for the line descent, which is the vertical distance
        // the pen travels from line to line. Setting it to zero will cause the lines to be drawn on top of
        // each-other, although I haven't tried it.
        map_size getBitmapHeight(const line_iterator &start, const line_iterator &end, float lineSpacingMod);
        // Returns the pixel width of the line range
        map_size getBitmapWidth(const line_iterator &start, const line_iterator &end);
        // Compute all values for line rendering, given a line range, `channels` number and the `lineSpacingMod`
        // `lineHeight` is the height of each line
        // `width` is the _pixel_ width of the line range
        // `height` is the _pixel_ height of the line range
        // `offset` is the starting draw offset for each line
        // `fall` is the value deducted from the offset after each line is drawn
        // so as to move to the next line.
        // To see the proper use of these values, see renderLines(...)
        void getLinesRenderData(const line_iterator &start, const line_iterator &end, int channels, float lineSpacingMod, map_size &lineHeight, map_size &width, map_size &height, int &offset, int &fall);
    };
}

#endif
