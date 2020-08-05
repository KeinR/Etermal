#include "BmpFont.h"

#include <stdexcept>
#include <cstring>

// Note: in stb_image.c, set to only bitmap files.
// That should help somewhat.
#include <stb/stb_image.h>

#include <stb/stb_image_write.h>

#include "opengl.h"

etm::BmpFont::BmpFont(
    const std::string &path,
    char_t startCodepoint,
    char_t glyphWidth,
    char_t glyphHeight,
    char_t count
):
    startCodepoint(startCodepoint),
    glyphs(count),
    width(glyphWidth),
    height(glyphHeight)
{
    GLint unpackAlign;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlign);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    stbi_set_flip_vertically_on_load(false);

    constexpr int channels = 1;
    int imgWidth, imgHeight;
    unsigned char *img = stbi_load(path.c_str(), &imgWidth, &imgHeight, NULL, channels);
    if (img == NULL) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
        throw std::invalid_argument("etm::BmpFont: Failed to load bitmap font \"" + path + "\"");
    }

    const int rows = imgHeight / height;
    const int columns = imgWidth / width;

    const int bufferSize = width * height * channels;
    unsigned char *buffer = new unsigned char[bufferSize];

    unsigned int glyphIndex = 0;

    bool doLoop = true;
    for (int y = 0; y < rows && doLoop; y++) {
        for (int x = 0; x < columns; x++) {
            const int offset = (x * width + y * height * imgWidth) * channels;
            // Clear buffer
            std::memset(buffer, 0, bufferSize);
            // Copy range
            for (int cy = 0; cy < height; cy++) {
                for (int cx = 0; cx < width; cx++) {
                    const int srcIndex = offset + (cx + cy * imgWidth) * channels;
                    const int insIndex = (cx + (height - 1 - cy) * width) * channels;
                    buffer[insIndex] = img[srcIndex];
                }
            }
            // Insert into texture
            glyphs[glyphIndex].setData(GL_RED, width, height, buffer);
            glyphIndex++;
            if (glyphIndex >= glyphs.size()) {
                doLoop = false;
                break;
            }
        }
    }

    delete[] buffer;
    stbi_image_free(img);
    if (unpackAlign != 1) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlign);
    }

    // Truncate remaining glyphs
    glyphs.resize(glyphIndex);
    glyphs.shrink_to_fit();
}

void etm::BmpFont::setResMan(Resources *res) {
    // Do nothing
}

void etm::BmpFont::setSize(unsigned int size) {
    // Do nothing - scaling bitmap fonts is a big no no.
    // Besides, it would require keeping source and
    // scaled coppies of the bitmap, and that's just lame.
}
void etm::BmpFont::bindChar(char_t c) {
    char_t index = c - startCodepoint + 1;
    if (index < glyphs.size()) {
        glyphs[index].bind();
    } else {
        glyphs[0].bind();
    }
}
void etm::BmpFont::clearCache() {
    // Do nothing, there's no cache
}
int etm::BmpFont::getCharWidth() {
    return width;
}
int etm::BmpFont::getCharHeight() {
    return height;
}
