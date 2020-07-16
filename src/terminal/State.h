#ifndef ETERMAL_STATE_H_INCLUDED
#define ETERMAL_STATE_H_INCLUDED

#include "util/singleton.h"
#include "render/glfw.h"

// render/Model
namespace etm { class Model; }

/*
* Upon construction, records the current state.
* Upon a call to set(), sets the OpenGL states
* desired for the terminal.
* Upon a call to restore(), restores the previous states
* that were recorded upon construction.
* May not be moved or copied, as that wouldn't make sense.
*/

namespace etm {
    class State: public singleton {
        GLboolean blend;
        GLint program;
        GLint unpackAlign;
    public:
        State();
        void set(const Model &viewport);
        void restore();
    };
}

#endif
