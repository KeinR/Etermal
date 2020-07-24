#ifndef ETERMAL_FRAMEBUFFER_H_INCLUDED
#define ETERMAL_FRAMEBUFFER_H_INCLUDED

namespace etm { class Texture; }

namespace etm {

    /**
    * Also known as an "off-screen window",
    * renderbuffers are particualarly useful
    * for generating textures from rendered
    * scenes and processing them.
    */
    class Framebuffer {
        /// Handle to the internal OpenGL buffer
        unsigned int handle;
        /**
        * Move the given Framebuffer into self.
        * @param [in,out] The framebuffer to move
        */
        void steal(Framebuffer &other);
        /**
        * Deletes the framebuffer
        */
        void free();
    public:
        /**
        * Stores and releases a framebuffer state.
        * Upon construction, stores the state.
        * Upon destruction, restores that state.
        * @see etm::State
        */
        class State {
            /// The write framebuffer
            int readFramebuffer;
            /// The read framebuffer
            int writeFramebuffer;
        public:
            /**
            * Constructs and calls @ref store()
            */
            State();
            /**
            * Constructs and calls @ref restore()
            */
            ~State();
            /**
            * Records the current framebuffer bindings.
            */
            void store();
            /**
            * Restores the recorded framebuffer bindings.
            */
            void restore();
        };

        /**
        * Construct a framebuffer and allocate a
        * new OpenGL framebuffer.
        */
        Framebuffer();
        /**
        * Move a framebuffer into this one.
        * @param [in,out] The framebuffer to move
        */
        Framebuffer(Framebuffer &&other);
        /**
        * Destructs and destroys the tied OpenGL
        * framebuffer.
        */
        ~Framebuffer();
        /**
        * Move a framebuffer into this one.
        * @param [in,out] The framebuffer to move
        * @return `*this`
        */
        Framebuffer &operator=(Framebuffer &&other);

        /**
        * Attach a texture for output.
        * @note The framebuffer will be bound after
        * this call.
        * @param [in] tex The texture to attach
        */
        void attachTexture(Texture &tex);

        /**
        * Bind this framebuffer globally.
        */
        void bind();
    };
}

#endif
