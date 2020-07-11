#include "Image.h"

#include <iostream>

#include <stb/stb_image.h>

#include "../util/error.h"
#include "../render/glfw.h"
#include "../Resources.h"

int etm::getImageFormat(int channels) {
    switch (channels) {
        case 4: return GL_RGBA;
        case 3: return GL_RGB;
        case 2: return GL_RG;
        case 1: return GL_RED;
        // Could cause a problem if, for some reason, one of the
        // above enums is def'd as 0.
        // I have no idea why they would do that though.
        default: return 0;
    }
}
unsigned char *etm::loadImagePath(const std::string &path, int &width, int &height, int &format) {
    // OpenGL's 0,0 is at the bottom-left hand corner of the screen, the opposate of images,
    // the origins of which are at the top-left
    stbi_set_flip_vertically_on_load(true);
    // The last parameter is the desired number of channels - I'm not sure what it does to get
    // the channel number to that value (can't just make data appear out of thin air, and deleting
    // data is just a no no). Hence we give 0 as the parameter, which disables the feature.
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &format, 0);
    // Not nullptr because, since stb_image is a C lib, it only has NULL.
    // Therefore, this is safer
    if (data == NULL) {
        throw fe_error("image.cpp::loadImagePath(const std::string&, int&, int&, int&): Failed to load image @" + path);
    }

    format = getImageFormat(format);
    if (!format) {
        stbi_image_free(data);
        throw fe_error("image.cpp::loadImagePath(const std::string&, int&, int&, int&): Image has bad number of channels; getImageFormat() returns 0 @" + path);
    }

    return data;
}
etm::man_image etm::loadImageStr(const std::string &path) {

    int width, height, format;
    unsigned char *data = loadImagePath(path, width, height, format);

    // TODO: Would having a shared pointer to just the image data be a better idea?
    return std::shared_ptr<imageData>(new imageData{format, width, height, data}, [](imageData *image) -> void {
        stbi_image_free(image->data);
        delete image;
    });
}
void etm::freeImage(unsigned char *data) {
    stbi_image_free(data);
}


etm::Image::Image(Resources *res): res(res) {
}

void etm::Image::loadImage(const std::string &path) {
    setImage(loadImageStr(path));
}

void etm::Image::setImage(const imageData &image) {
    texture.setData(image.format, image.width, image.height, image.data);
}

void etm::Image::setImage(const man_image &image) {
    texture.setData(image->format, image->width, image->height, image->data);
}

void etm::Image::setImage(int format, int width, int height, unsigned char *data) {
    texture.setData(format, width, height, data);
}

void etm::Image::setX(int x) {
    model.x = x;
}
void etm::Image::setY(int y) {
    model.y = y;
}
void etm::Image::setWidth(int width) {
    model.width = width;
}
void etm::Image::setHeight(int height) {
    model.height = height;
}

int etm::Image::getX() {
    return model.x;
}
int etm::Image::getY() {
    return model.y;
}
int etm::Image::getWidth() {
    return model.width;
}
int etm::Image::getHeight() {
    return model.height;
}

etm::Texture &etm::Image::getTexure() {
    return texture;
}

void etm::Image::render() {
    // Insert the model into the shader uniform
    model.set(res->getShader());
    // Bind the texture so that it's the one that's sampled from
    texture.bind();
    // Render using the configuration in the buffer
    res->renderTexRectangle();
}
