#include "Font.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>
#include <stdexcept>

#include "../Resources.h"
#include "opengl.h"
#include "FontLibrary.h"

etm::Font::Font(const std::string &path): res(nullptr) {
    FT_Error error = FT_New_Face(fontLib.get(), path.c_str(), 0, &face);
    if (error == FT_Err_Ok) {
        setSize(18); // Default size
    } else {
        face = nullptr;
        throw std::invalid_argument(
            "etm::Font: Failed to load font @\"" + path +
            "\" as new face (ft err code = " + std::to_string(error) +
            ")");
    }
}
etm::Font::Font(Font &&other) {
    steal(other);
}
etm::Font::~Font() {
    free();
}

etm::Font &etm::Font::operator=(Font &&other) {
    steal(other);
    return *this;
}

void etm::Font::steal(Font &other) {
    res = other.res;
    face = other.face;
    charWidth = other.charWidth;
    charHeight = other.charHeight;
    textCache = std::move(other.textCache);

    other.face = nullptr;
}

void etm::Font::free() {
    // If face is nullptr, that's a flag
    // that it was either moved or construciton failed.
    if (face != nullptr) {
        FT_Done_Face(face);
    }
}

void etm::Font::setResMan(Resources *res) {
    this->res = res;
}

void etm::Font::calcCharSize() {
    charWidth = std::ceil(face->size->metrics.max_advance / 64.0f);
    // https://www.freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_facerec
    // "If you want the global glyph height, use ascender - descender"
    charHeight = std::ceil((face->size->metrics.ascender - face->size->metrics.descender) / 64.0f);   
}

void etm::Font::setSize(unsigned int size) {
    FT_Set_Pixel_Sizes(face, 0, size);
    calcCharSize();
    clearCache();
}
etm::Texture etm::Font::makeCharTexture(char_t c) {
    // Only one channel is needed to convey font data
    constexpr int channels = 1;

    Texture result;

    FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if (error == FT_Err_Ok) {

        std::vector<unsigned char> data(charHeight * charWidth * channels);
        // Move down the codepoint down by the difference between it's and the max ascender
        const int yShift = face->size->metrics.ascender / 64 - face->glyph->bitmap_top;
        // Center the codepoint in the texture
        const int xShift = (charWidth - face->glyph->bitmap.width) / 2;
        for (unsigned int sy = 0; sy < face->glyph->bitmap.rows; sy++) {
            for (unsigned int sx = 0; sx < face->glyph->bitmap.width; sx++) {
                // The source index
                const int srcIndex = static_cast<int>(sx) + static_cast<int>(sy) * face->glyph->bitmap.width;
                // Only render into `render`'s bitmap if it's got content.
                // Note that the Freetype bitmaps only have one channel,
                // hence why there's only one check.
                if (face->glyph->bitmap.buffer[srcIndex]) {
                    // The insertion index.
                    // Flipts the texture because OpenGL operates
                    // with 0,0 at the lower left corner.
                    // 
                    // (
                    //     (xShift + sx) // Add pixel x offset to the pen x offset 
                    //     + // Add to offset from vertical coords
                    //     (
                    //         charHeight // Total line height
                    //         - // Remove 1 to zero-index it
                    //         1
                    //         - // Remove the y coords to get the vertically fliped index
                    //         (yShift + sy) // Add pixel y coord to the pen y coord
                    //     )
                    //     * // Convert from pixel coords to bitmap offset
                    //     charWidth // Offset per y
                    // )
                    // * // Multply by desired channels
                    // channels // Number of channels
                    // 
                    const int insIndex = ((xShift + static_cast<int>(sx)) + (charHeight - 1 - (yShift + static_cast<int>(sy))) * charWidth) * channels;
                    for (int c = 0; c < channels; c++) {
                        data.at(insIndex + c) = face->glyph->bitmap.buffer[srcIndex];
                    }
                }
            }
        }

        result.setData(GL_RED, charWidth, charHeight, data.data());

    } else if (res != nullptr) {
        res->postError(
            "Font::renderChar(char)",
            std::string("Failed to load char '") + std::to_string(c) + "'",
            error,
            false
        );
    } else {
        std::cerr << "ETERMAL ERROR: etm::Font::renderChar(char): Not only is the resources manager nullptr, but codepoint "
        << c << " failed to render with freetype error code " << error << '\n';
    }

    return result;
}

void etm::Font::bindChar(char_t c) {
    // Test cache
    textCache_t::iterator loc = textCache.find(c);
    if (loc != textCache.end()) {
        loc->second.bind();
    } else {
        textCache[c] = makeCharTexture(c);
        textCache[c].bind();
    }
}

void etm::Font::clearCache() {
    textCache.clear();
}

int etm::Font::getCharWidth() {
    return charWidth;
}
int etm::Font::getCharHeight() {
    return charHeight;
}
