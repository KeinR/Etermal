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
    * @see Terminal
    */
    class State: public singleton {
        /// GL_DEPTH_TEST toggle value
        GLboolean depth;
        /// GL_CURRENT_PROGRAM value
        GLint program;
        /// GL_UNPACK_ALIGNMENT value
        GLint unpackAlign;
    public:
        /**
        * Construct a State.
        * @warning Fields are in an undefined state; that
        * is, they're default initialized.
        */
        State();
        /**
        * Store the current state into fields
        */
        void store();
        /**
        * Sets the desired state.
        * @note This relies on the values stored with @ref store()
        * @see restore()
        */
        void set();
        /**
        * Store the state described in `*this`'s fields.
        * @see store()
        * @see set()
        */
        void restore();
    };
}

#endif
