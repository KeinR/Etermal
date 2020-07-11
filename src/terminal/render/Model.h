#ifndef ETERMAL_MODEL_H_INCLUDED
#define ETERMAL_MODEL_H_INCLUDED

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm {
    class Model {
    public:
        float x;
        float y;
        float width;
        float height;

        // Initializes all values to zero
        Model();
        // Initialize with values
        Model(float x, float y, float width, float height);

        bool hasPoint(float x, float y);

        void set(const shader::Shader &shader);
    };
}

#endif
