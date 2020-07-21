#include "Texture.h"

#include <string>

#include "opengl.h"

// In-use enum values for Texture.
// def'd to be extra safe.
#define TEXTURE_TYPE GL_TEXTURE_2D
#define TEXTURE_PIXEL_TYPE GL_UNSIGNED_BYTE

// Default parameters.
constexpr etm::Texture::tparam etm::Texture::defaultParams{
    GL_CLAMP_TO_BORDER,
    GL_CLAMP_TO_BORDER,
    GL_NEAREST,
    GL_NEAREST
};

etm::Texture::Texture() {
    init(defaultParams);
}

etm::Texture::Texture(const tparam &param) {
    init(param);
}

etm::Texture::~Texture() {
    deInit();
}

void etm::Texture::init(const tparam &params) {
    glGenTextures(1, &buffer);

    setParams(params);
}

void etm::Texture::deInit() {
    glDeleteTextures(1, &buffer);
}

void etm::Texture::steal(Texture &other) {
    buffer = other.buffer;
    other.buffer = 0;
}

etm::Texture::Texture(Texture &&other) {
    steal(other);
}
etm::Texture &etm::Texture::operator=(Texture &&other) {
    steal(other);
    return *this;
}

unsigned int etm::Texture::get() {
    return buffer;
}

void etm::Texture::bind() const {
    glBindTexture(TEXTURE_TYPE, buffer);
}

void etm::Texture::setData(int format, unsigned int width, unsigned int height, const data_t *data) {
    bind();
    glTexImage2D(TEXTURE_TYPE, 0, format, width, height, 0, format, TEXTURE_PIXEL_TYPE, data);
}

void etm::Texture::setDefaultParams() {
    setParams(defaultParams);
}
void etm::Texture::setParams(const tparam &params) {
    bind();

    // x/y wrap parameter
    glTexParameteri(TEXTURE_TYPE, GL_TEXTURE_WRAP_S, params.xWrap);
    glTexParameteri(TEXTURE_TYPE, GL_TEXTURE_WRAP_T, params.yWrap);

    // min/max filtering parameter
    glTexParameteri(TEXTURE_TYPE, GL_TEXTURE_MIN_FILTER, params.minFilter);
    glTexParameteri(TEXTURE_TYPE, GL_TEXTURE_MAG_FILTER, params.maxFilter);
}
