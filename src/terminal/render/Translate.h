#ifndef ETERMAL_TRANSLATE_H_INCLUDED
#define ETERMAL_TRANSLATE_H_INCLUDED

#include <glm/glm.hpp>

namespace etm {
    // Model
    class Model;
    // RModel
    class RModel;
    // Resources
    class Resources;
}

namespace etm::tsl {
    glm::mat4 model(Resources *res, Model &m);
    glm::mat4 rModel(Resources *res, RModel &m);
}

#endif
