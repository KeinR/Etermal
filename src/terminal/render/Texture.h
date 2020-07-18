#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

/*
* Textures are used to represent... Well, textures.
* Same thing as images.
*/

namespace etm {
    class Texture {
    public:
        typedef unsigned char data_t;
        // tparam's parameters are used to give
        // OpenGL filtering configurations to
        // the texture.
        // x/yWrap specify how to wrap the texture.
        // Possible values are GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, or GL_REPEAT
        // min/max filter specifies how to filter the texture when it's being
        // scaled down or up respectively.
        // Possible values are GL_NEAREST and GL_LINEAR
        struct tparam {
            int xWrap;
            int yWrap;

            int minFilter;
            int maxFilter;
        };
    private:
        // Handle to the corresponding OpenGL buffer
        unsigned int buffer;

        // Initialize with parameters
        void init(const tparam &param);
        // Deinitialize and free resources
        void deInit();
        void steal(Texture &other);
    public:
        // Takes default tparam
        Texture();
        // Takes provided tparam
        Texture(const tparam &params);
        ~Texture();
        Texture(Texture &&other);
        Texture &operator=(Texture &&other);

        // Gets the id of the underlying OpenGL buffer
        unsigned int get();

        // Sets this texture as current, so that it'll be used in
        // whatever texture slot is active (via glActiveTexture(...))
        void bind() const;
        // Set the content of the texture
        // `format` is a OpenGL enum value https://www.khronos.org/opengl/wiki/Image_Format
        // `width` and `height` are the dimensions of the texture/imaeg, and `data` is a pointer
        // to the raw data
        void setData(int format, unsigned int width, unsigned int height, const data_t *data);
        void setData(int textureType, int format, unsigned int width, unsigned int height, const data_t *data);

        // Resets the filtering and wrapping parameters to their defaults
        void setDefaultParams();
        // Changes the filtering and wrapping parameters
        void setParams(const tparam &params);
    };
}

#endif
