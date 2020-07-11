#include "Buffer.h"

#include <iostream>

#include "glfw.h"

etm::Buffer::Buffer(initfunc_t initFunc) {
    setInit(initFunc);
    gen();
    init();
}

etm::Buffer::~Buffer() {
    deGen();
}

etm::Buffer::Buffer(Buffer &&other) {
    steal(other);
}
etm::Buffer::Buffer(const Buffer &other) {
    // Only have to generate two because the
    // vertex array is generated in copy(...)
    glGenBuffers(1, &vertices);
    glGenBuffers(1, &indices);
    copy(other);
}
etm::Buffer &etm::Buffer::operator=(Buffer &&other) {
    deGen();
    steal(other);
    return *this;
}
etm::Buffer &etm::Buffer::operator=(const Buffer &other) {
    // The vertex array is regenerated, and since this's buffers have
    // allready been generated, the vertex array must be deleted.
    glDeleteVertexArrays(1, &array);
    copy(other);
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

void etm::Buffer::copy(const Buffer &other) {
    // Assumes that the array element array buffers
    // are already generated, and that the vertex array
    // object has been either deleted or not generated yet.

    // Generate a new vertex array object to clear old settings
    glGenVertexArrays(1, &array);

    // Get size of other's array buffer
    int vertSize;
    other.bindVert();
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vertSize);

    // Force this's array buffer to allocate a buffer
    // large enough to hold the data from other
    const int verticiesLen = vertSize / sizeof(float);
    float *verticiesData = new float[verticiesLen];
    setVerticies(verticiesLen, verticiesData);
    delete[] verticiesData;

    // Copy other's array buffer into this's array buffer.
    glBindBuffer(GL_COPY_READ_BUFFER, other.vertices);
    glBindBuffer(GL_COPY_WRITE_BUFFER, vertices);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vertSize);

    // Get size of other's element array buffer
    int elemSize;
    other.bindElem();
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &elemSize);

    // Force this's element array buffer to allocate a buffer
    // large enough to hold the data from other
    const int indicesLen = elemSize / sizeof(unsigned int);
    unsigned int *indicesData = new unsigned int[indicesLen];
    setIndices(indicesLen, indicesData);
    delete[] indicesData;

    // Copy other's element array buffer into this's
    glBindBuffer(GL_COPY_READ_BUFFER, other.indices);
    glBindBuffer(GL_COPY_WRITE_BUFFER, indices);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, elemSize);

    // Trivial copies
    countIndices = other.countIndices;
    initFunc = other.initFunc;

    // Attempt to call the new initialization function
    // to setup the vertex array object
    init();
}

void etm::Buffer::gen() {
    glGenVertexArrays(1, &array);
    glGenBuffers(1, &vertices);
    glGenBuffers(1, &indices);
    countIndices = 0;
}

void etm::Buffer::deGen() {
    if (array) {
        glDeleteVertexArrays(1, &array);
        glDeleteBuffers(1, &vertices);
        glDeleteBuffers(1, &indices);
    }
    countIndices = 0;
}

void etm::Buffer::setInit(initfunc_t initFunc) {
    this->initFunc = initFunc;
}

void etm::Buffer::init() {
    // `initFunc` is equal to nullptr
    // if it's not been set
    if (initFunc != nullptr) {
        bind();
        initFunc(*this);
    }
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
    std::cout << "countIndices = " << countIndices << std::endl;
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
    // glDrawElements(GL_TRIANGLES, countIndices, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINES, countIndices, GL_UNSIGNED_INT, 0);
}
