#include "Buffer.h"

#include "opengl.h"

etm::Buffer::Buffer() {
    gen();
}

etm::Buffer::~Buffer() {
    deGen();
}

etm::Buffer::Buffer(Buffer &&other) {
    steal(other);
}
etm::Buffer &etm::Buffer::operator=(Buffer &&other) {
    deGen();
    steal(other);
    return *this;
}

void etm::Buffer::steal(Buffer &other) {
    array = other.array;
    vertices = other.vertices;
    indices = other.indices;
    countIndices = other.countIndices;
    other.array = 0;
    other.vertices = 0;
    other.indices = 0;
    other.countIndices = 0;
}

void etm::Buffer::gen() {
    glGenVertexArrays(1, &array);
    glGenBuffers(1, &vertices);
    glGenBuffers(1, &indices);
    countIndices = 0;
}

void etm::Buffer::deGen() {
    // If `array` is zero, signals that it was moved.
    if (array) {
        glDeleteVertexArrays(1, &array);
        glDeleteBuffers(1, &vertices);
        glDeleteBuffers(1, &indices);
    }
    countIndices = 0;
}

void etm::Buffer::bindArray() const {
    glBindVertexArray(array);
}
void etm::Buffer::bindVert() const {
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
}
void etm::Buffer::bindElem() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
}
void etm::Buffer::bind() const {
    bindArray();
    bindVert();
    bindElem();
}

void etm::Buffer::setVerticies(int count, float *data) {
    bindVert();
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_STATIC_DRAW);
}
void etm::Buffer::setIndices(int count, unsigned int *data) {
    countIndices = count;
    bindElem();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}
void etm::Buffer::setParam(unsigned int index, int size, int stride, int offset) {
    bind();
    // Load the settings
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
    // Apply the settings
    glEnableVertexAttribArray(index);
}

void etm::Buffer::render() {
    bind();
    // Draw the verticies in the array buffer according to the
    // indices in the element buffer
    glDrawElements(GL_TRIANGLES, countIndices, GL_UNSIGNED_INT, 0);
}
