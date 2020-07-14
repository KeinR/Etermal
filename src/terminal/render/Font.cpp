#include "Font.h"

#include <cmath>
#include <iostream>
#include <vector>

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
etm::Texture etm::Font::renderChar(char c) {

    // std::cout << "font self = " << this << std::endl;

    constexpr int channels = 4;
    // TEMP
    float channelMod[channels] = {
        191.0f / 255.0f,
        191.0f / 255.0f,
        191.0f / 255.0f,
        1
    };

    const int lineHeight = face->size->metrics.height / 64;
    const int uWidth = face->size->metrics.max_advance / 64;

    FT_Error error = FT_Load_Char(face, c, FT_LOAD_RENDER);
    if (error == FT_Err_Ok) {

        std::vector<unsigned char> data(lineHeight * uWidth * channels);
        const int yShift = face->size->metrics.ascender / 64 - face->glyph->bitmap_top;
        const int xShift = (uWidth - face->glyph->bitmap.width) / 2;
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
                    const int insIndex = ((xShift + static_cast<int>(sx)) + (lineHeight - 1 - (yShift + static_cast<int>(sy))) * uWidth) * channels;
                    // Apply the bitmap value to every channel in the insertion bitmap in `render`
                    for (int c = 0; c < channels; c++) {
                        data.at(insIndex + c) = face->glyph->bitmap.buffer[srcIndex];
                    }
                }
            }
        }

        Texture tex;
        tex.setData(GL_RGBA, uWidth, lineHeight, data.data());

        return tex;
    } else {
        throw fe_error("Failed to load glyph: " + std::to_string(error));
    }
}
