#ifndef ETERMAL_TEXTURE_H_INCLUDED
#define ETERMAL_TEXTURE_H_INCLUDED

namespace etm {

    /**
    * A wrapper for an OpenGL texture object.
    * Can specify texture parameters through the constructor
    * [@ref Texture(const tparam &params)] or later on
    * [@ref setParams(const tparam &params)].
    * If parameters are not specified, uses the defaults
    * [@ref defaultParams]
    */
    class Texture {
    public:
        /// Type of data accepted by the Texture
        /// @see setData(int format, unsigned int width, unsigned int height, const data_t *data)
        typedef unsigned char data_t;

        /**
        * tparam's parameters are used to give
        * OpenGL filtering configurations to
        * the texture.
        * x/yWrap specify how to wrap the texture.
        * Possible values are GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, or GL_REPEAT
        * min/max filter specifies how to filter the texture when it's being
        * scaled down or up respectively.
        * Possible values are GL_NEAREST and GL_LINEAR
        * @see setDefaultParams()
        * @see setParams(const tparam &params)
        */
        struct tparam {
            /// OpenGL enum for GL_TEXTURE_WRAP_S
            int xWrap;
            /// OpenGL enum for GL_TEXTURE_WRAP_T
            int yWrap;

            /// OpenGL enum for GL_TEXTURE_MIN_FILTER
            int minFilter;
            /// OpenGL enum for GL_TEXTURE_MAG_FILTER
            int maxFilter;
        };

        /// The default parameters.
        /// @see setDefaultParams()
        static const tparam defaultParams;

    private:
        /// Handle to the managed OpenGL buffer
        unsigned int buffer;

        /**
        * Initializes the Texture.
        * Creates the actual @ref buffer
        * @param [in] params Parameters to be set
        */
        void init(const tparam &param);

        /**
        * Frees managed resources.
        * @see buffer
        */
        void deInit();

        /**
        * Takes ownership of `other`'s @ref buffer
        * @param [in,out] other Target
        */
        void steal(Texture &other);
    public:
        /**
        * Creates a texture object and applies the default parameters.
        * @see setDefaultParams()
        */
        Texture();
        /**
        * Creates a texture object and applies given parameters.
        * @param [in] params Texture parameters
        * @see setParams(const tparam &params)
        */
        Texture(const tparam &params);
        /**
        * Destuct the texture object, destroying the managed
        * texture object - if it hasn't been moved.
        */
        ~Texture();
        /**
        * Initialize by taking ownership of `other`'s buffer
        * @param [in,out] other Target
        */
        Texture(Texture &&other);
        /**
        * Take ownership of `other`'s buffer,
        * and destroy `*this`'s
        * @param [in,out] other Target
        */
        Texture &operator=(Texture &&other);

        /**
        * Gets the wrapped OpenGL buffer, @ref buffer.
        * @return @ref buffer
        */
        unsigned int get();

        /**
        * Sets this texture as current, so that it'll be used in
        * whatever texture slot is active (via glActiveTexture(...))
        */
        void bind() const;

        /**
        * Set the content of the texture.
        * @param [in] format OpenGL enum value describing the data format [<a href="https://www.khronos.org/opengl/wiki/Image_Format">wiki</a>]
        * @param [in] width The @e pixel width of the texture
        * @param [in] height The @e pixel height of the texture
        * @param [in] data Pointer to the raw pixel data.
        */
        void setData(int format, unsigned int width, unsigned int height, const data_t *data);

        /**
        * Resets the filtering and wrapping parameters to their defaults
        * @see defaultParams
        */
        void setDefaultParams();
        /**
        * Changes the filtering and wrapping parameters
        * @param [in] params The desiried parameters
        * @see tparam
        */
        void setParams(const tparam &params);
    };
}

#endif
