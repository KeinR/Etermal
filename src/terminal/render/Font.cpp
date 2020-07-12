#include "Font.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "../gui/Image.h"
#include "../util/error.h"
#include "glfw.h"

etm::Font::Font(FontLibrary &lib, const std::string &path) {
    FT_Error error = FT_New_Face(lib.get(), path.c_str(), 0, &face);
    if (error != FT_Err_Ok) {
        throw fe_error("etm::Font::Font(const std::string&): Failed to load font @\"" + path + "\" as new face: " + std::to_string(error));
    }
    setSize(16);
}
etm::Font::~Font() {
    free();
}

void etm::Font::free() {
    if (face != nullptr) {
        FT_Done_Face(face);
    }
}

FT_Face etm::Font::getFace() {
    return face;
}

void etm::Font::setSize(unsigned int size) {
    FT_Set_Pixel_Sizes(face, 0, size);
}
void etm::Font::renderChar(int x, int y, char c, Image &out) {

    // std::cout << "font self = " << this << std::endl;

    constexpr int channels = 4;
    // TEMP
    float channelMod[channels] = {
        1,
        1,
        1,
        1
    };

    const int lineHeight = face->size->metrics.height / 64;
    const int uWidth = face->size->metrics.max_advance / 64;

    FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if (error == FT_Err_Ok) {
        // Round the current pen positions,
        // make space for the left side bound (x),
        // and do the same for the ascent (y).
        // This makes the pen go deeper into the bitmap, preventing
        // buffer underflow from the bitmap being rendered too far left
        // or above the pen
        // const int ix = x + face->glyph->bitmap_left;
        // const int iy = y - face->glyph->bitmap_top;

        if (c == 'a') {
            std::cout << "break" << std::endl;
        }

        std::vector<unsigned char> data(lineHeight * uWidth * channels);
        const int yDiff = lineHeight - face->glyph->bitmap.rows;
        // std::cout << "face->glyph->bitmap.rows = " << face->glyph->bitmap.rows << std::endl;
        // std::cout << "face->glyph->bitmap.width = " << face->glyph->bitmap.width << std::endl;
        // std::cout << "lineHeight = " << lineHeight << std::endl;
        // std::cout << "uWidth = " << uWidth << std::endl;
        // unsigned char *data = new unsigned char[face->glyph->bitmap.rows * face->glyph->bitmap.width * 4];
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
                    //         render.lineHeight // Total line height
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
                    const int insIndex = (static_cast<int>(sx) + (lineHeight - 1 - (yDiff + static_cast<int>(sy))) * uWidth) * channels;
                    // Apply the bitmap value to every channel in the insertion bitmap in `render`
                    for (int c = 0; c < channels; c++) {
                        // Modify the channel value by the corresponding 0-1 float value in channelMod
                        data.at(insIndex + c) = std::round(face->glyph->bitmap.buffer[srcIndex] * channelMod[c]);
                    }
                }
            }
        }

        out.setImage(GL_RGBA, uWidth, lineHeight, data.data());

        // delete[] data;
    } else {
        throw fe_error("Failed to load glyph: " + std::to_string(error));
    }
}
