#ifndef ETERMAL_ROTATABLEMODEL_H_INCLUDED
#define ETERMAL_ROTATABLEMODEL_H_INCLUDED

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm {
    class RModel {
    public:
        float x;
        float y;
        float width;
        float height;
        float rotation;

        // Initializes all values to zero
        RModel();
        // Initialize with values
        RModel(float x, float y, float width, float height, float rotation);

        void set(const shader::Shader &shader);
    };
}

#endif
