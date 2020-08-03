#ifndef ETERMAL_STATE_H_INCLUDED
#define ETERMAL_STATE_H_INCLUDED

#include "util/singleton.h"
#include "render/opengl.h"

// render/Model
namespace etm { class Model; }

namespace etm {

    /**
    * Used to keep track of states desired by the terminal,
    * set them, and restore their old values.
    * Is a @ref singleton because copying a State class
    * just wouldn't make any sense.
    * Upon construction, captures all state values.
    * upon destruction, resores those values.
    * This is to prevent a state from being messed
    * up because someone decided to throw.
    * @see Terminal
    */
    class State: public singleton {
        /// GL_DEPTH_TEST toggle value
        GLboolean depth;
        /// GL_BLEND toggle value
        GLboolean blend;
        /// GL_CURRENT_PROGRAM value
        GLint program;
        /// GL_UNPACK_ALIGNMENT value
        GLint unpackAlign;
        /// GL_ACTIVE_TEXTURE value
        GLint activeTexture;
    public:
        /**
        * Constructs and calls @ref store()
        */
        State();
        /**
        * Destructs and calls @ref restore()
        */
        ~State();
        /**
        * Manually store the current state into fields
        * @see State()
        */
        void store();
        /**
        * Sets the desired state.
        * @note This relies on the values stored with @ref store()
        * @see restore()
        */
        void set();
        /**
        * Manually restore the state described in `*this`'s fields.
        * @see ~State()
        * @see store()
        * @see set()
        */
        void restore();
    };
}

#endif
