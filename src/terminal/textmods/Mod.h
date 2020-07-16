#ifndef ETERMAL_TM_MOD_H_INCLUDED
#define ETERMAL_TM_MOD_H_INCLUDED

// TextState
namespace etm::tm { class TextState; }

namespace etm::tm {
    class Mod {
    public:
        virtual ~Mod() = 0;
        virtual void run(TextState &state) = 0;
    };
}

#endif
