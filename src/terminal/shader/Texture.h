#ifndef ETERMAL_SHADER_TEXT_H_INCLUDED
#define ETERMAL_SHADER_TEXT_H_INCLUDED

#include "Shader.h"

namespace etm::shader {
    class Texture: public Shader {
        uniform_t model;
    public:
        Texture();
        uniform_t getModel() const override;
        uniform_t getColor() const override;
    };
}

#endif
