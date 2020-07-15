#ifndef ETERMAL_SHADER_PRIMITIVE_H_INCLUDED
#define ETERMAL_SHADER_PRIMITIVE_H_INCLUDED

#include "Shader.h"

namespace etm::shader {
    class Primitive: public Shader {
        uniform_t model;
        uniform_t color;
    public:
        Primitive();
        uniform_t getModel() const override;
        uniform_t getColor() const override;
        uniform_t getBackGColor() const override;
        uniform_t getForeGColor() const override;
    };
}

#endif
