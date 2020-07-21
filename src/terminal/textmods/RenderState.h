#ifndef ETERMAL_TM_RENDERSTATE_H_INCLUDED
#define ETERMAL_TM_RENDERSTATE_H_INCLUDED

#include "TextState.h"

namespace etm {
    // ../render/Color
    class Color;
    namespace shader {
        // ../shader/Shader
        class Shader;
    }
}

namespace etm::tm {

    /**
    * Describes and maintains the state of text as
    * it is being rendered.
    * @see Lookbehind
    * @see TextBuffer
    */
    class RenderState: public TextState {
        /// The shader in which back/foreground
        /// uniform data is stored
        shader::Shader *shader;
        /// The current background color
        const Color *backgroundColor;
        /// The default background color
        const Color *const defBackgroundColor;
        /// The current foreground color
        const Color *foregroundColor;
        /// The default foreground color
        const Color *const defForegroundColor;
        /// Are the background and foreground colors inverted?
        /// @see setInverted(bool val)
        bool inverted;

        /**
        * Set the color as the background, and load it
        * into the corresponding uniform
        * @warning Takes a pointer to the color data,
        * so the given color object must stay allocated until the
        * background changes again.
        * @param [in] color The color to set
        */
        void doSetBack(const Color &color);
        /**
        * Set the color as the foreground, and load it
        * into the corresponding uniform
        * @warning Takes a pointer to the color data,
        * so the given color object must stay allocated until the
        * foreground changes again.
        * @param [in] color The color to set
        */
        void doSetFore(const Color &color);
    public:
        RenderState(shader::Shader &shader, const Color &defBackgroundColor, const Color &defForegroundColor, bool startInverted);

        /**
        * Set default background color as the background.
        * Basically calls `setBack(*defBackgroundColor)`
        * @see setBack(const Color &color)
        */
        void setDefBack() override;
        /**
        * Set default foreground color as the foreground.
        * Basically calls `setFore(*defForegroundColor)`
        * @see setFore(const Color &color)
        */
        void setDefFore() override;

        /**
        * Set the color as the background, and load it
        * into the corresponding uniform.
        * Does not check duplicates.
        * If inverted, this becomes the foreground.
        * @warning Takes a pointer to the color data,
        * so the given color object must stay allocated until the
        * background changes again.
        * @param [in] color The color to set
        */
        void setBack(const Color &color) override;
        /**
        * Set the color as the foreground, and load it
        * into the corresponding uniform.
        * Does not check duplicates.
        * If inverted, this becomes the background.
        * @warning Takes a pointer to the color data,
        * so the given color object must stay allocated until the
        * foreground changes again.
        * @param [in] color The color to set
        */
        void setFore(const Color &color) override;

        /**
        * Set whether the foreground and background colors
        * should be swapped.
        * @param [in] val `true` if the colors should be swapped
        */
        void setInverted(bool val);
    };
}

#endif
