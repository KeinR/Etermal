#ifndef ETERMAL_SHADER_TEXT_H_INCLUDED
#define ETERMAL_SHADER_TEXT_H_INCLUDED

#include "Shader.h"

namespace etm::shader {
    class Text: public Shader {
        Resources *res;
        uniform_t backgroundColor;
        uniform_t foregroundColor;
        uniform_t model;
    public:
        Text(Resources *res);
        uniform_t getModel() const override;
        uniform_t getColor() const override;
        uniform_t getForeGColor() const override;
        uniform_t getBackGColor() const override;
    };
}

#endif
