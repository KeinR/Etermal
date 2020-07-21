#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>

#include "../util/singleton.h"

namespace etm { class Resources; }

namespace etm::shader {
    /// Global type used to represent shader uniform locations
    typedef int uniform_t;

    /**
    * Extensible interface used to load and manage shaders.
    * @see Primitive
    * @see Text
    */
    class Shader: public singleton {
    public:
        /// Type used to store the shader program
        typedef unsigned int shader_program_t;
    private:
        /// Handle/id of the wrapped OpenGL shader object
        shader_program_t shader;
        /*
        * Destroys the managed shader
        */
        void free();
    public:
        /**
        * Constructs initializes a Shader object.
        * @param [in] res A Resources object to report errors to
        * @param [in] vertexData Pointer to the raw vertex shader data
        * @param [in] vertexLen length of `vertexData`
        * @param [in] fragData Pointer to the raw fragment shader data
        * @param [in] fragLen length of `fragData`
        */
        Shader(Resources *res, const char *vertexData, int vertexLen, const char *fragData, int fragLen);
        virtual ~Shader() = 0;

        // Sets this shader as current
        void use() const;
        // Gets the handle/id of this shader
        shader_program_t get() const;

        /**
        * Get the location of the shader's model uniform.
        * @return location of the model uniform
        */
        virtual uniform_t getModel() const = 0;
        /**
        * Get the location of the shader's color uniform.
        * @return location of the color uniform
        */
        virtual uniform_t getColor() const = 0;
        /**
        * Get the location of the shader's background color uniform.
        * @return location of the background color uniform
        */
        virtual uniform_t getBackGColor() const = 0;
        /**
        * Get the location of the shader's foreground color uniform.
        * @return location of the foreground color uniform
        */
        virtual uniform_t getForeGColor() const = 0;
    };
}

#endif
