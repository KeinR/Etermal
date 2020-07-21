#ifndef ETERMAL_MODEL_H_INCLUDED
#define ETERMAL_MODEL_H_INCLUDED

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm {

    /**
    * A simple structure that represents the position
    * and size of an object.
    * @see RModel
    * @see Color
    */
    class Model {
    public:
        /// X coordinate
        float x;
        /// Y coordinate
        float y;
        /// Width
        float width;
        /// Height
        float height;

        /**
        * Construct a Model and initialize all values to zero
        */
        Model();
        /**
        * Construct a Model and initialize all values
        * @param [in] x X value
        * @param [in] x Y value
        * @param [in] x Width value
        * @param [in] x Height value
        */
        Model(float x, float y, float width, float height);

        /**
        * Returns true if the model contains the given point.
        * @param [in] x The x value
        * @param [in] y The y value
        * @return `true` if the point is contained within the Model
        */
        bool hasPoint(float x, float y) const;

        /**
        * Generates a model using the @ref x, @ref y,
        * @ref width, and @ref height fields, and sets
        * it to the current shader's uniform location
        * specified by the given shader.
        * @param [in] shader Shader from which to get the model location from
        */
        void set(const shader::Shader &shader);
    };
}

#endif
