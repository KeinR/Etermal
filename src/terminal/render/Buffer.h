#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <cstddef>

/*
* The basic level of rendering, buffers hold references to
* OpenGL buffers and deal with vertex data.
* 
* Because of how vertex array objects, (OpenGL objects that store
* information about how to interpret vertex data), hold
* references to specific buffers, (making it impossible to
* re-use them for other buffers) an initialization function is
* optionally taken by the buffer so as to allow for copy semantics.
* This function takes the buffer itself and is supposed to initialize
* the vertex array object of the buffer through setParam(...).
* Yes, this does mean that copies of a buffer without a set initialization
* function will be ill-formed, because their vertex array objects won't
* be properly initialized!
*/

namespace etm {
    class Buffer {
    public:
        // Custom initialization function
        typedef void(*initfunc_t)(Buffer &buffer);
    private:
        // The vertex array object
        unsigned int array;
        // The array buffer
        unsigned int vertices;
        // The element array buffer
        unsigned int indices;

        // The number of indices contained in `indices`
        int countIndices;

        // Initialization function for vertex arrays
        // to enable copying of this object
        initfunc_t initFunc;

        void copy(const Buffer &other);
        void steal(Buffer &other);
        // Generate buffers & vertex array
        void gen();
        // Delete buffers & vertex array
        void deGen();
    public:
        // NB: If copied while init is nullptr, the copied will be ill-formed!
        Buffer(initfunc_t init = nullptr);
        ~Buffer();
        Buffer(Buffer &&other);
        Buffer(const Buffer &other);
        Buffer &operator=(Buffer &&other);
        Buffer &operator=(const Buffer &other);

        // Set the initialzation function
        void setInit(initfunc_t initFunc);
        // Call the initialzation function - if it's set.
        void init();

        // Bind vertex array object
        void bindArray() const;
        // Bind array buffer
        void bindVert() const;
        // Bind element array buffer
        void bindElem() const;
        // Bind all
        void bind() const;

        // Set verticies for array buffer
        void setVerticies(int count, float *data);
        // Set indices for element array buffer
        void setIndices(int count, unsigned int *data);
        // Set parameter for vertex array object
        void setParam(unsigned int index, int size, int stride, int offset);

        // Render the indices
        void render();
    };
}

#endif
