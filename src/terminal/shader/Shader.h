#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <string>

#include "../util/singleton.h"

/*
* Shaders, useful little programs that process data
* before it's rendered.
* These have to be bound to be used via use(), and when they are,
* they're bound/used globally.
* Since the process of getting shader uniform locations is slow,
* a system of uniform "qualifications" is implemented, where
* a shader will have functions designed to access possible
* uniforms that are stored in a map, with the shader's id
* being used as the key.
* If the required uniform location is not found, depending on the
* function called, an "error" value (-1) is returned, or an
* exception is thrown. Either way, this allows code to do a runtime
* assert that a shader has certain uniforms.
*/

namespace etm::shader {
    typedef int uniform_t;

    class Shader: public singleton {
    public:
        // Actual shader handle/id type
        typedef unsigned int shader_program_t;
    private:
        // Handle/id of OpenGL shader object
        shader_program_t shader;
    protected:
        // Frees all managed resources
        void free();
        void steal(Shader &other);
    public:
        // Throws in instance of "fe_error" if fail
        Shader(const char *vertexData, int vertexLen, const char *fragData, int fragLen);
        virtual ~Shader() = 0;

        // Sets this shader as current
        void use() const;
        // Gets the handle/id of this shader
        shader_program_t get() const;

        // Subclasses override these
        virtual uniform_t getModel() const = 0;
        virtual uniform_t getColor() const = 0;
        virtual uniform_t getBackGColor() const = 0;
        virtual uniform_t getForeGColor() const = 0;
    };
}

#endif
