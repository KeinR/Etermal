#ifndef ETERMAL_BUFFER_H_INCLUDED
#define ETERMAL_BUFFER_H_INCLUDED

namespace etm {

    /**
    * The basic level of rendering, Buffers hold references to
    * OpenGL buffers and deal with vertex data.
    */
    class Buffer {
    private:
        /// ID of the vertex array object.
        unsigned int array;
        /// ID of the array buffer.
        unsigned int vertices;
        /// ID of the element array buffer.
        unsigned int indices;

        /// The number of indices contained in @ref indices.
        /// Used during @ref render()
        int countIndices;

        /**
        * Move the contents of another Buffer into self.
        * @param [in,out] other The target Buffer
        */
        void steal(Buffer &other);

        /**
        * Generate the buffers & vertex array.
        */
        void gen();

        /**
        * Delete the buffers & vertex array.
        */
        void deGen();
    public:
        /**
        * Constructs a buffer object.
        * @note Creates OpenGL buffers in the current context.
        */
        Buffer();
        /**
        * Destruct buffer.
        * Destroys OpenGL buffers with the id's
        * contained in @ref array, @ref verticies,
        * and @ref indices, if it hasn't been moved. 
        */
        ~Buffer();
        /**
        * Initialize with moved object.
        * @param [in,out] other Target object
        */
        Buffer(Buffer &&other);
        /**
        * Move object into `*this`.
        * @param [in,out] other Target object
        */
        Buffer &operator=(Buffer &&other);

        /**
        * Bind vertex array object.
        */
        void bindArray() const;
        /**
        * Bind array buffer.
        */
        void bindVert() const;
        /**
        * Bind element array buffer.
        */
        void bindElem() const;
        /**
        * Bind all.
        * @see bindArray()
        * @see bindVert()
        * @see bindElem()
        */
        void bind() const;

        /**
        * Set verticies for array buffer.
        * @param [in] count Number of verticies
        * @param [in] data Pointer to vertex data
        * @see vertices
        */
        void setVerticies(int count, float *data);

        /**
        * Set indices for element array buffer.
        * @param [in] count Number of indices
        * @param [in] data Pointer to index data
        * @see indices
        */
        void setIndices(int count, unsigned int *data);

        /**
        * Set parameter for vertex array object.
        * @param [in] index Attribute index
        * @param [in] size Number of values in each attribute
        * @param [in] stride Distance travelled to get to the next attribute
        * @param [in] offset Index of the first value
        * @see array
        */
        void setParam(unsigned int index, int size, int stride, int offset);

        /**
        * Render the @ref indices with the @ref array and @ref verticies
        * to the current framebuffer.
        * @note @ref setVerticies(int count, float *data), 
        * @ref setIndices(int count, unsigned int *data), and
        * @ref setParam(unsigned int index, int size, int stride, int offset)
        * must be called to initialize the Buffer, or else an OpenGL error may
        * be set.
        * @note Must be rendered in the same context it was created in.
        */
        void render();
    };
}

#endif
