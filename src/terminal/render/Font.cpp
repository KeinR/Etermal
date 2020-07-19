#include "Font.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "../Resources.h"
#include "opengl.h"
#include "FontLibrary.h"

etm::Font::Font(Resources *res, FontLibrary &lib, const std::string &path): res(res) {
    FT_Error error = FT_New_Face(lib.get(), path.c_str(), 0, &face);
    if (error != FT_Err_Ok) {
        face = nullptr;
        res->postError(
            "etm::Font::Font(Resources*,FontLibrary&,const std::string&)",
            "Failed to load font @\"" + path + "\" as new face",
            error,
            true
        );
    } else {
        setSize(16);
    }
}
etm::Font::~Font() {
    free();
}

void etm::Font::free() {
    if (face != nullptr) {
        FT_Done_Face(face);
    }
}

void etm::Font::calcCharSize() {
    charWidth = face->size->metrics.max_advance / 64;   
    charHeight = face->size->metrics.height / 64;   
}

void etm::Font::setSize(unsigned int size) {
    FT_Set_Pixel_Sizes(face, 0, size);
    calcCharSize();
    clearCache();
}
etm::Texture etm::Font::makeCharTexture(char c) {
    constexpr int channels = 1;

    Texture result;

    FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if (error == FT_Err_Ok) {

        std::vector<unsigned char> data(charHeight * charWidth * channels);
        const int yShift = face->size->metrics.ascender / 64 - face->glyph->bitmap_top;
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
                    //     (ix + sx) // Add pixel x offset to the pen x offset 
                    //     + // Add to offset from vertical coords
                    //     (
                    //         charHeight // Total line height
                    //         - // Remove 1 to zero-index it
                    //         1
                    //         - // Remove the y coords to get the vertically fliped index
                    //         (iy + sy) // Add pixel y coord to the pen y coord
                    //     )
                    //     * // Convert from pixel coords to bitmap offset
                    //     render.verticalStride // Offset per y
                    // )
                    // * // Multply by desired channels
                    // render.channels // Number of channels
                    // 
                    const int insIndex = ((xShift + static_cast<int>(sx)) + (charHeight - 1 - (yShift + static_cast<int>(sy))) * charWidth) * channels;
                    for (int c = 0; c < channels; c++) {
                        data.at(insIndex + c) = face->glyph->bitmap.buffer[srcIndex];
                    }
                }
            }
        }

        result.setData(GL_RED, charWidth, charHeight, data.data());

    } else {
        res->postError(
            "Font::renderChar(char)",
            std::string("Failed to load char '") + c + "'",
            error,
            false
        );
    }

    return result;
}

void etm::Font::bindChar(char c) {
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
