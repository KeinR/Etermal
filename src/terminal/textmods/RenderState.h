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
    class RenderState: public TextState {
        shader::Shader *shader;
        const Color *backgroundColor;
        const Color *const defBackgroundColor;
        const Color *foregroundColor;
        const Color *const defForegroundColor;
    public:
        RenderState(shader::Shader &shader, const Color &defBackgroundColor, const Color &defForegroundColor);

        // Sets default background color, constant
        void setDefBack() override;
        // Sets default foreground color, constant
        void setDefFore() override;

        // Set the background color
        void setBack(const Color &color) override;
        // Set the foreground color
        void setFore(const Color &color) override;
    };
}

#endif