#ifndef ETERMAL_TM_MOD_H_INCLUDED
#define ETERMAL_TM_MOD_H_INCLUDED

// TextState
namespace etm::tm { class TextState; }

namespace etm::tm {

    /**
    * Extensible interface for a @ref TextState modifier.
    */
    class Mod {
    public:
        virtual ~Mod() = 0;

        /**
        * Modifies the state how it sees fit
        * @param [in,out] state The state to modify
        */
        virtual void run(TextState &state) = 0;
    };
}

#endif
