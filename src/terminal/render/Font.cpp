#include "Font.h"

#include <iostream>
#include <cmath>
#include <sstream>
#include <type_traits> // For static assert

#include "ftype.h"

#include "../util/error.h"

#define ASSERT_ERR(def) \
    def " is not typedef'd the same as " def "_for, must fix forward defs"

static_assert(
    std::is_same<FT_Face, etm::Font::FT_Face_for>::value,
    ASSERT_ERR("FT_Face")
);
static_assert(
    std::is_same<FT_Library, etm::Font::FT_Library_for>::value,
    ASSERT_ERR("FT_Library")
);
static_assert(
    std::is_same<FT_Error, etm::Font::FT_Error_for>::value,
    ASSERT_ERR("FT_Error")
);
static_assert(
    std::is_same<FT_Pos, etm::Font::FT_Pos_for>::value,
    ASSERT_ERR("FT_Pos")
);

#undef ASSERT_ERR

// Our own custom implementation of a newline,
// because fonts generally represent newlines as
// nul characters
constexpr unsigned int newline = -1;

// Round 26.6 fractional pixels off to 32 bit integers
inline static int rto32(int v26_6) {
    return (v26_6 >> 6) + ((v26_6 & 0x3F) < 32 ? 0 : 1);
}

// Reformat lsb values to where they are positive and in 26.6
// fractional pixel format, so that they can be used as x-offsets.
inline static int compXOfs(int lsb) {
    return -lsb << 6;
}

// Compute the x offset for a line given its `width`, `wrappingWidth`, and the desired text align
inline static int compOffset(int lsb, etm::Font::textAlign align, unsigned int &width, unsigned int wrappingWidth) {
    etm::Font::ofs_type xOffset = compXOfs(lsb);
    // TODO: The performance impact is negligible, but is there a better way,
    // like using function pointers?
    switch (align) {
        case etm::Font::textAlign::left:
            break;
        case etm::Font::textAlign::center: {
            // TODO: Perhaps ceil?
            etm::Font::ofs_type shift = (wrappingWidth - width) / 2;
            xOffset += shift;
            width += shift;
            std::cout << "center, shift = " << shift << std::endl;
            break;
        }
        case etm::Font::textAlign::right:
            xOffset += wrappingWidth - width;
            width = wrappingWidth;
            std::cout << "right, wrappingWidth = " << wrappingWidth << std::endl;
            break;
    }
    return xOffset;
}

// Start struct constructors

etm::Font::line::line():
    horizontalOffset(0), width(0) {
}
etm::Font::line::line(const glyph_iterator &start, const glyph_iterator &end, ofs_type horizontalOffset, map_size width):
    start(start), end(end), horizontalOffset(horizontalOffset), width(width) {
}

etm::Font::renderConf::renderConf():
    channels(0), cMod(0), bitmap(0), verticalStride(0), lineHeight(0) {
}
etm::Font::renderConf::renderConf(const int channels, float *cMod, unsigned char *bitmap, unsigned int verticalStride, map_size lineHeight):
    channels(channels), cMod(cMod), bitmap(bitmap), verticalStride(verticalStride), lineHeight(lineHeight) {
}

etm::Font::vec2::vec2(): x(0), y(0) {
}
etm::Font::vec2::vec2(map_size x, map_size y): x(x), y(y) {
}

etm::Font::glyph_render_fail::glyph_render_fail(int code) noexcept {
    std::stringstream stream;
    stream << "Freetype error 0x" << std::hex << code << std::dec <<
    " while decoding glyph";
    str = stream.str();
}
const char *etm::Font::glyph_render_fail::what() const noexcept {
    return str.c_str();
}

// End struct constructors

etm::Font::Font(FT_Library_for lib, const std::string &fontPath) {

    FT_Error error = FT_New_Face(lib, fontPath.c_str(), 0, &face);
    if (error != FT_Err_Ok) {
        throw fe_error("etm::Font::Font(const std::string&): Failed to load font @\"" + fontPath + "\" as new face: " + std::to_string(error));
    }
    space = FT_Get_Char_Index(face, ' ');
    fontNewline = FT_Get_Char_Index(face, '\n');

    setSize(defaultSize);
}

etm::Font::~Font() {
    free();
}

etm::Font::glyph_type etm::Font::getGlyph(glyph_type glyph) {
    // `newline` is our control value, see file header
    if (glyph == newline) {
        return fontNewline;
    }
    return glyph;
}

void etm::Font::free() {
    if (face != nullptr) {
        FT_Done_Face(face);
    }
}

void etm::Font::steal(Font &other) {
    face = other.face;
    other.face = nullptr;
    copyPOD(other);
}

void etm::Font::copyPOD(const Font &other) {
    space = other.space;
    fontNewline = other.fontNewline;
}

etm::Font::Font(Font &&other) {
    steal(other);
}

etm::Font &etm::Font::operator=(Font &&other) {
    free();
    steal(other);
    return *this;
}

FT_Face etm::Font::getFace() {
    return face;
}

etm::Font::FT_Error_for etm::Font::setSize(unsigned int size) {
    return FT_Set_Pixel_Sizes(face, 0, size);
}

void etm::Font::stringMetrics(glyph_iterator start, const glyph_iterator &end, map_size *width, map_size *height, ofs_type *horizontalOffset) {
    if (start >= end) {
        return;
    }
    // Width from the advance x and horizontal offset, 26.6 fractional
    map_size preWidth = 0;
    // Height from the advance y and actual height of the characters, 26.6 fractional
    map_size preHeight = 0;
    // The horizontal offset, 26.6 fractional
    ofs_type preHorizontalOffset = 0;
    // The height of the actual glyphs
    map_size sHeight = 0;
    for (;; ++start) {
        const glyph_type g = getGlyph(*start);
        FT_Error error = FT_Load_Glyph(face, g, FT_LOAD_BITMAP_METRICS_ONLY);
        if (error == FT_Err_Ok) {
            preWidth += face->glyph->advance.x;
            preHeight += face->glyph->advance.y;
            if (face->glyph->metrics.height > sHeight) {
                sHeight = face->glyph->metrics.height;
            }
            // lsb
            if (face->glyph->metrics.horiBearingX < preHorizontalOffset) {
                preHorizontalOffset = face->glyph->metrics.horiBearingX;
            }
            if (start + 1 < end) {
                FT_Vector kerning;
                FT_Get_Kerning(face, g, getGlyph(*(start + 1)), FT_KERNING_UNFITTED, &kerning);
                preWidth += kerning.x;
                preHeight += kerning.y;
            } else {
                break;
            }
        } else {
            throw glyph_render_fail(error);
        }
    }
    if (width != nullptr) {
        // Deduct the lsb to the width to account for horizontal offset
        *width = rto32(preWidth - preHorizontalOffset);
    }
    if (height != nullptr) {
        // Add any height gain from the advance y to the height of the glyphs
        *height = rto32(preHeight + sHeight);
    }
    if (horizontalOffset != nullptr) {
        *horizontalOffset = -preHorizontalOffset;
    }
}

int etm::Font::stringDescent(glyph_iterator start, const glyph_iterator &end) {
    int max = 0;
    for (; start < end; ++start) {
        FT_Error error = FT_Load_Glyph(face, getGlyph(*start), FT_LOAD_BITMAP_METRICS_ONLY);
        if (error == FT_Err_Ok) {
            // We aren't explicitly given the descent, so we have to compute it ourselves
            // by deducting the ascent from the height 
            const int lower = face->glyph->metrics.height - face->glyph->metrics.horiBearingY;
            if (lower > max) {
                max = lower;
            }
        } else {
            throw glyph_render_fail(error);
        }
    }
    return max;
}

etm::Font::map_size etm::Font::stringAscent(glyph_iterator start, const glyph_iterator &end) {
    map_size max = 0;
    for (; start < end; ++start) {
        FT_Error error = FT_Load_Glyph(face, getGlyph(*start), FT_LOAD_BITMAP_METRICS_ONLY);
        if (error == FT_Err_Ok) {
            if (face->glyph->metrics.horiBearingY > max) {
                max = face->glyph->metrics.horiBearingY;
            }
        } else {
            throw glyph_render_fail(error);
        }
    }
    return max;
}

std::vector<etm::Font::char_metrics> etm::Font::measureString(glyph_iterator start, const glyph_iterator &end) {
    std::vector<char_metrics> result;
    if (start >= end) {
        return result;
    }
    result.reserve(end - start);
    for (;; ++start) {
        const glyph_type g = getGlyph(*start);
        FT_Error error = FT_Load_Glyph(face, g, FT_LOAD_BITMAP_METRICS_ONLY);
        if (error == FT_Err_Ok) {
            result.push_back(char_metrics{face->glyph->bitmap_left, 0, face->glyph->advance.x});
            if (start + 1 < end) {
                FT_Vector kerning;
                // FT_KERNING_UNFITTED so that we get the most accurate kern values - 
                // 26.6 fractional pixels, not fitted to the pixel grid (not rounded)
                FT_Get_Kerning(face, g, getGlyph(*(start + 1)), FT_KERNING_UNFITTED, &kerning);
                result.back().kern = kerning.x;
            } else {
                break;
            }
        } else {
            throw glyph_render_fail(error);
        }
    }
    return result;
}

unsigned char *etm::Font::renderBitmap(const glyph_iterator &start, const glyph_iterator &end, const int channels, float cMod[], map_size &width, map_size &height) {
    int horizontalOffset;
    stringMetrics(start, end, &width, &height, &horizontalOffset);
    unsigned char *bitmap = new unsigned char[width * height * channels]{0};
    renderTo(start, end, renderConf(channels, cMod, bitmap, width, height), horizontalOffset);
    return bitmap;
}

void etm::Font::renderTo(glyph_iterator start, const glyph_iterator &end, const renderConf &render, ofs_type horizontalOffset) {
    if (start >= end) {
        return;
    }
    vec2 pen(horizontalOffset, 0);
    for (;; ++start) {
        if (start + 1 < end) {
            renderGlyphTo(*start, *(start + 1), render, pen);
        } else {
            renderGlyphTo(*start, 0, render, pen);
            break;
        }
    }
}

void etm::Font::renderGlyphTo(glyph_type glyph, glyph_type next, const renderConf &render, vec2 &pen) {
    FT_Error error = FT_Load_Glyph(face, getGlyph(glyph), FT_LOAD_RENDER);
    if (error == FT_Err_Ok) {
        // Round the current pen positions,
        // make space for the left side bound (x),
        // and do the same for the ascent (y).
        // This makes the pen go deeper into the bitmap, preventing
        // buffer underflow from the bitmap being rendered too far left
        // or above the pen
        const int ix = rto32(pen.x) + face->glyph->bitmap_left;
        const int iy = rto32(pen.y) - face->glyph->bitmap_top;
        // unsigned char *debugmap = new unsigned char[face->glyph->bitmap.rows * face->glyph->bitmap.width];
        for (unsigned int y = 0; y < face->glyph->bitmap.rows; y++) {
            for (unsigned int x = 0; x < face->glyph->bitmap.width; x++) {
                // The source index
                const int srcIndex = static_cast<int>(x) + static_cast<int>(y) * face->glyph->bitmap.width;
                // Only render into `render`'s bitmap if it's got content.
                // Note that the Freetype bitmaps only have one channel,
                // hence why there's only one check.
                if (face->glyph->bitmap.buffer[srcIndex]) {
                    // The insertion index.
                    // Flipts the texture because OpenGL operates
                    // with 0,0 at the lower left corner.
                    // 
                    // (
                    //     (ix + x) // Add pixel x offset to the pen x offset 
                    //     + // Add to offset from vertical coords
                    //     (
                    //         render.lineHeight // Total line height
                    //         - // Remove 1 to zero-index it
                    //         1
                    //         - // Remove the y coords to get the vertically fliped index
                    //         (iy + y) // Add pixel y coord to the pen y coord
                    //     )
                    //     * // Convert from pixel coords to bitmap offset
                    //     render.verticalStride // Offset per y
                    // )
                    // * // Multply by desired channels
                    // render.channels // Number of channels
                    // 
                    const int insIndex = ((ix + static_cast<int>(x)) + (render.lineHeight - 1 - (iy + static_cast<int>(y))) * render.verticalStride) * render.channels;
                    // Apply the bitmap value to every channel in the insertion bitmap in `render`
                    for (int c = 0; c < render.channels; c++) {
                        // Modify the channel value by the corresponding 0-1 float value in cMod 
                        render.bitmap[insIndex + c] = std::round(face->glyph->bitmap.buffer[srcIndex] * render.cMod[c]);
                    }
                }
            }
        }

        // Move the pen to the next location
        pen.x += face->glyph->advance.x;
        pen.y += face->glyph->advance.y;
        // If there's another suceeding glyph, apply kerning.
        if (next) {
            FT_Vector kerning;
            FT_Get_Kerning(face, glyph, next, FT_KERNING_UNFITTED, &kerning);
            pen.x += kerning.x;
            pen.y += kerning.y;
        }
    } else {
        throw glyph_render_fail(error);
    }
}

unsigned char *etm::Font::renderLines(line_iterator start, const line_iterator &end, const int channels, float cMod[], float lineSpacingMod, map_size &width, map_size &height) {
    // See getLinesRenderData(...) for a description of these values
    map_size lineHeight;
    int offset;
    int fall;
    getLinesRenderData(start, end, channels, lineSpacingMod, lineHeight, width, height, offset, fall);

    unsigned char *bitmap = new unsigned char[width * height * channels]{0};

    // Construct default render configuration, with everything but the bitmap,
    // which will be added later
    renderConf render(channels, cMod, nullptr, width, lineHeight);
    for (; start < end; ++start) {

        render.bitmap = bitmap + offset;
        renderTo(start->start, start->end, render, start->horizontalOffset);

        offset -= fall;
    }
    return bitmap;
}

etm::Font::line etm::Font::makeLine(const glyph_iterator &start, const glyph_iterator &end) {
    line result;
    result.start = start;
    result.end = end;
    // Line doesn't need height, ergo it's `nullptr`
    stringMetrics(start, end, &result.width, nullptr, &result.horizontalOffset);
    // The returned width is a normal 32 bit integer, so we need to convert `width`
    // to 26.6 to be compatable
    result.width <<= 6;
    return result;
}

etm::Font::line_str etm::Font::getLines(const glyph_iterator &start, const glyph_iterator &end, unsigned int wrappingWidth, const textAlign align) {
    line_str lines;
    // Must check early because we do bounds checks late
    if (start >= end) {
        return lines;
    }
    wrappingWidth <<= 6; // Convert to 26.6 format for compatability; NOTE this could cause problems with values over 2^26-1
    std::vector<char_metrics> metrics = measureString(start, end);
    unsigned int width = compXOfs(metrics[0].lsb); // TODO: Change data type to something more reasonable, and std::max(width, 0)
    // Was there a space not involved in a line break recently?
    // This determines if we do hard or soft wrap.
    bool spaceLast = false;
    // The start of the line, inclusive
    glyph_iterator begin = start;
    for (glyph_iterator it = start; it < end; ++it) {
        // Forced line break, indicated by newline control value (see file header)
        if (*it == newline) {

            // The range that will be ignored
            glyph_iterator stop = it; // exclusive
            glyph_iterator resume = it+1; // inclusive

            // compXOfs(...) -> Get the lsb from the glyph at the start of the line.
            // This is important because we need to shift the pen at the start so that characters
            // like j, which write to the left of the pen (at least with Arial...) don't cause a
            // buffer underflow or poor text quality
            lines.emplace_back(begin, stop, compXOfs(metrics[begin - start].lsb), width);
            begin = resume;
            width = compXOfs(metrics[resume - start].lsb);
        } else {
            int i = it - start;
            const int move = metrics[i].advance + metrics[i].kern;
            if (width + move > wrappingWidth) {
                glyph_iterator stop = it;
                glyph_iterator resume = it;
                unsigned int thisWidth = width;
                // Check if we should do soft or hard wrap - 
                // if there's a space char here, there's no harm in doing a hard wrap.
                // Same for if there was space previously
                if (*it == space) {
                    if ((it <= begin || *(it-1) != space) && (it+1 >= end || *(it+1) != space)) {
                        // If there's no space preceeding or suceedings this one, ignore it
                        ++resume;
                        // Don't take the extra advance from the current space, because we ignore it
                        width = 0;
                    } else {
                        width = move;
                    }
                } else {
                    width = move;
                    if (spaceLast) {
                        // Given that there was a space before this char that can
                        // be wrapped to...
                        spaceLast = false;
                        // Backtrack to the last space for soft wrap
                        for (glyph_iterator newI = stop; newI >= start; --newI) {
                            if (*newI == space) {
                                ++newI;
                                resume = newI;
                                // For each part that the frame has been shifted,
                                // transfer the corresponding character width
                                for (int fi; stop > newI;) {
                                    --stop;
                                    fi = stop - start;
                                    thisWidth -= metrics[fi].advance + metrics[fi].kern;
                                    width += metrics[fi].advance + metrics[fi].kern;
                                }
                                stop = newI;
                                // If there's a space that should be deleted
                                if (stop <= begin || *(stop-1) != space) {
                                    --stop;
                                    int fi = stop - start;
                                    thisWidth -= metrics[fi].advance + metrics[fi].kern;
                                }
                                break;
                            }
                        }
                    }
                }

                ofs_type xOffset = compOffset(metrics[begin - start].lsb, align, thisWidth, wrappingWidth);
                lines.emplace_back(begin, stop, xOffset, thisWidth);
                begin = resume;
                // Init the width with the lsb of the first char
                width += compXOfs(metrics[resume - start].lsb);
            } else {
                width += move;
                if (*it == space) {
                    spaceLast = true;
                }
            }
        }
    }



    std::cout << "line widht = " << width << std::endl;
    ofs_type xOffset = compOffset(metrics[begin - start].lsb, align, width, wrappingWidth);
    lines.emplace_back(begin, end, xOffset, width);
    std::cout << "lines.size() = " << lines.size() << std::endl;
    std::cout << "lines.back() = " << lines.back().width << std::endl;

    return lines;
}

etm::Font::glyph_str etm::Font::indexString(std::string::const_iterator start, const std::string::const_iterator &end) {
    glyph_str index;
    index.reserve(end - start);
    for (; start < end; ++start) {
        if (*start == '\n') {
            // If it's a newline, we have a special value for it...
            index.push_back(newline);
        } else {
            index.push_back(FT_Get_Char_Index(face, *start));
        }
    }
    return index;
}

std::vector<etm::Font::line_iterator> etm::Font::breakupLines(line_iterator start, const line_iterator &end, float lineSpacingMod, map_size maxHeight) {
    std::vector<etm::Font::line_iterator> result;

    // The ammount at the foot of the lines that can be deducted due to how there doesn't need to
    // be spacing for the next line
    const map_size deduction = rto32(std::round(face->size->metrics.height * (lineSpacingMod - 1)));
    // Distance per line
    const map_size descent = rto32(std::round(face->size->metrics.height * lineSpacingMod));

    // Increase the max height
    maxHeight += deduction;

    while (start < end) {
        for (map_size height = 0;; height += descent) {
            if (height <= maxHeight) {
                ++start;
                if (start >= end) {
                    result.push_back(end);
                    break;
                }
            } else {
                result.push_back(start);
                break;
            }
        }
    }

    return result;
}

etm::Font::line_iterator etm::Font::truncLines(line_iterator start, const line_iterator &end, float lineSpacingMod, map_size maxHeight) {
    // Same deal as breakupLines(...), except this one returns on the first instance of height exceeding
    // the max height

    const int deduction = rto32(std::round(face->size->metrics.height * (lineSpacingMod - 1)));
    const int descent = rto32(std::round(face->size->metrics.height * lineSpacingMod));

    maxHeight += deduction;

    for (unsigned int height = 0;; height += descent) {
        if (height <= maxHeight) {
            ++start;
            if (start >= end) {
                return end;
            }
        } else {
            return start;
        }
    }
}

etm::Font::map_size etm::Font::getBitmapHeight(const line_iterator &start, const line_iterator &end, float lineSpacingMod) {
    // The ammount at the foot of the lines that can be deducted due to
    // how there doesn't need to be spacing for the next line
    const int deduction = rto32(std::round(face->size->metrics.height * (lineSpacingMod - 1)));
    // Distance per line
    const int descent = rto32(std::round(face->size->metrics.height * lineSpacingMod));
    return (end - start) * descent - deduction;
}

etm::Font::map_size etm::Font::getBitmapWidth(const line_iterator &start, const line_iterator &end) {
    // Search through the lines and find the greatest width
    map_size width = 0;
    for (line_iterator it = start; it < end; ++it) {
        if (it->width > width) {
            width = it->width;
        }
    }
    // Round off the width from 26.6 to 32
    return rto32(width);
}

void etm::Font::getLinesRenderData(const line_iterator &start, const line_iterator &end, int channels, float lineSpacingMod, map_size &lineHeight, map_size &width, map_size &height, int &offset, int &fall) {
    if (start >= end) {
        return;
    }
    // The ascent of the first line
    const etm::Font::map_size cap = stringAscent(start->start, start->end);
    // https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_size_metrics
    // "The height in 26.6 fractional pixels, rounded to an integer value"
    // Rounded to the nearest pixel coordinate or 26.6 fractional value?
    // Eh, best to be on the safe side. 
    etm::Font::map_size descent = std::round(face->size->metrics.height * lineSpacingMod); // 26.6

    // Hight per line
    lineHeight = rto32(descent);
    // bitmap width
    width = getBitmapWidth(start, end);
    // This deduction saves a little bit of memory; deduct the foot of empty space at the bottom of the line
    height = rto32((end - start - 1) * descent + cap + stringDescent((end-1)->start, (end-1)->end));
    // Start bitmap offset
    offset = (height - rto32(descent + cap)) * width * channels;
    // Offset deduction per line
    fall = width * channels * lineHeight;
}
