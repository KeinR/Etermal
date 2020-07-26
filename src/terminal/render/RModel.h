#ifndef ETERMAL_ROTATABLEMODEL_H_INCLUDED
#define ETERMAL_ROTATABLEMODEL_H_INCLUDED

#include "Model.h"

namespace etm {
    namespace shader {
         // ../shader/Shader
        class Shader;
    }
    // ../Resources
    class Resources;
}

namespace etm {

    /**
    * A rotatable model.
    * Almost a duplicate of @ref Model
    * because I don't want glm to be included
    * by Terminal.
    * @see Model
    * @see Color
    */
    class RModel: public Model {
    public:
        /// Rotation in degrees
        float rotation;

        /**
        * Construct a RModel and initialize all values to zero
        */
        RModel();
        /**
        * Construct a Model and initialize all values
        * @param [in] x X value
        * @param [in] x Y value
        * @param [in] x Width value
        * @param [in] x Height value
        * @param [in] rotation Rotation value
        */
        RModel(float x, float y, float width, float height, float rotation);

        /**
        * Generates a model using the @ref x, @ref y,
        * @ref width, @ref height and @ref rotation fields, and sets
        * it to the current shader's uniform location
        * specified by the given shader.
        * @param [in] res Resources manager from which to get the model location and viewport info from
        */
        void set(Resources *res);
    };
}

#endif
