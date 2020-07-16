#ifndef ETERMAL_TEXTSTATE_H_INCLUDED
#define ETERMAL_TEXTSTATE_H_INCLUDED

#include "../render/Color.h"

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm::tm {
    class TextState {
        shader::Shader *shader;
        const Color *backgroundColor;
        const Color *defBackgroundColor;
        const Color *foregroundColor;
        const Color *defForegroundColor;
    public:

        TextState(shader::Shader &shader, const Color &defBackgroundColor, const Color &defForegroundColor);
        shader::Shader &getShader();

        const Color &getBack();
        const Color &getDefBack();
        const Color &getFore();
        const Color &getDefFore();

        void setFore(const Color &Color);
        void setBack(const Color &Color);
    };
}

#endif
