#ifndef ETERMAL_TEXTMODS_MODS_CLUSTER_H_INCLUDED
#define ETERMAL_TEXTMODS_MODS_CLUSTER_H_INCLUDED

#include "Mod.h"

#include "../render/Color.h"

namespace etm::tm {

    class Background: public Mod {
        Color color;
    public:
        Background(const Color &color);
        void run(TextState &state) override;
    };

    class Foreground: public Mod {
        Color color;
    public:
        Foreground(const Color &color);
        void run(TextState &state) override;
    };

    class RevBackground: public Mod {
    public:
        RevBackground();
        void run(TextState &state) override;
    };

    class RevForeground: public Mod {
    public:
        RevForeground();
        void run(TextState &state) override;
    };

    class RevBackFore: public Mod {
    public:
        RevBackFore();
        void run(TextState &state) override;
    };

}

#endif
