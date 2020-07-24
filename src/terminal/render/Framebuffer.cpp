#include "Framebuffer.h"

#include "Texture.h"
#include "opengl.h"

etm::Framebuffer::State::State() {
    store();
}
etm::Framebuffer::State::~State() {
    restore();
}
void etm::Framebuffer::State::store() {
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFramebuffer);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &writeFramebuffer);
}
void etm::Framebuffer::State::restore() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFramebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, writeFramebuffer);
}

etm::Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &handle);
}
etm::Framebuffer::Framebuffer(Framebuffer &&other) {
    steal(other);
}
etm::Framebuffer::~Framebuffer() {
    free();
}
etm::Framebuffer &etm::Framebuffer::operator=(Framebuffer &&other) {
    free();
    steal(other);
    return *this;
}

void etm::Framebuffer::steal(Framebuffer &other) {
    handle = other.handle;
    other.handle = 0;
}
void etm::Framebuffer::free() {
    glDeleteFramebuffers(1, &handle);
}

void etm::Framebuffer::attachTexture(Texture &tex) {
    bind(); // We don't know what was bound before
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.get(), 0);
}

void etm::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
}

