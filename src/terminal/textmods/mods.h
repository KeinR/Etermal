#ifndef ETERMAL_TEXTMODS_MODS_CLUSTER_H_INCLUDED
#define ETERMAL_TEXTMODS_MODS_CLUSTER_H_INCLUDED

#include "Mod.h"

#include "../render/Color.h"

namespace etm::tm {

    /**
    * Changes the background of the @ref TextState
    */
    class Background: public Mod {
        Color color;
    public:
        /**
        * Constructs a background mod.
        * @param [in] color The color to change the state's background to
        */
        Background(const Color &color);
        /**
        * Sets the state's background color to @ref color
        * @param [out] state The state to modify
        */
        void run(TextState &state) override;
    };

    /**
    * Changes the foreground of the @ref TextState
    */
    class Foreground: public Mod {
        Color color;
    public:
        /**
        * Constructs a foreground mod.
        * @param [in] color The color to change the state's foreground to
        */
        Foreground(const Color &color);
        /**
        * Sets the state's foreground color to @ref color
        * @param [out] state The state to modify
        */
        void run(TextState &state) override;
    };

    /**
    * Reverts the background of the @ref TextState to the default
    * @see RevForeground
    * @see Revert
    */
    class RevBackground: public Mod {
    public:
        /**
        * Constructs a RevBackground mod.
        */
        RevBackground();
        /**
        * Changes the state's background color the default for that state
        * @param [out] state The state to modify
        */
        void run(TextState &state) override;
    };

    /**
    * Reverts the foreground of the @ref TextState to the default
    * @see RevBackground
    * @see Revert
    */
    class RevForeground: public Mod {
    public:
        /**
        * Constructs a RevForeground mod.
        */
        RevForeground();
        /**
        * Changes the state's foreground color the default for that state
        * @param [out] state The state to modify
        */
        void run(TextState &state) override;
    };

    /**
    * Reverts all styles of the @ref TextState to their defaults.
    * @see RevBackground
    * @see RevForeground
    */
    class Revert: public Mod {
    public:
        /**
        * Constructs a Revert mod.
        */
        Revert();
        /**
        * Changes the state's colors and any other styles to their defaults.
        * @param [out] state The state to modify
        */
        void run(TextState &state) override;
    };

}

#endif
