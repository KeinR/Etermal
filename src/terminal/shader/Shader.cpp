#include "Shader.h"

#include <string>
#include <fstream>

#include "../render/opengl.h"
#include "../Resources.h"

/**
* Compiles the given shader and returns a handle to the OpenGL object.
* @param [in] type The type (GL_VERTEX_SHADER/GL_FRAGMENT_SHADER)
* @param [in] data Pointer to shader data
* @param [in] length Length of `data`
* @param [out] error String to dump errors. Untouched if no errors occurred.
* @return ID of the newly created shader object, or 0 on error [<a href="http://docs.gl/es3/glCreateShader">ref</a>]
*/
static GLuint compileShader(GLenum type, const char *data, int length, std::string &error);

/**
* Links two shaders (vertex and fragment) and returns the resulting
* shader program.
* @param [in] vertObject The vertex object
* @param [in] fragObject The fragment object
* @param [out] error String to dump errors. Untouched if no errors occurred.
* @return ID of the resulting shader program, or [undefined] on error (I don't see it specified anywhere in the docs(?))
*/
static GLuint linkShaders(GLuint vertObject, GLuint fragObject, std::string &error);

etm::shader::Shader::Shader(Resources *res, const char *vertData, int vertLen, const char *fragData, int fragLen) {
    std::string error;

    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertData, vertLen, error);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragData, fragLen, error);
    if (error.size()) {
        error = "Failed to compile shaders: " + error;
        goto cleanup;
    }
    shader = linkShaders(vertShader, fragShader, error);
    if (error.size()) {
        error = "Failed to link shaders: " + error;
    }

    cleanup:
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // Prevent memory leaks if the error
    // callback decides to throw
    if (error.size()) {
        res->postError(
            "shader::Shader::Shader(Resources*,const char*,int,const char*,int)",
            error,
            0,
            true
        );
    }
}
etm::shader::Shader::~Shader() {
    free();
}
void etm::shader::Shader::free() {
    if (shader) {
        glDeleteProgram(shader);
    }
}

void etm::shader::Shader::use() const {
    glUseProgram(shader);
}

etm::shader::Shader::shader_program_t etm::shader::Shader::get() const {
    return shader;
}

GLuint compileShader(GLenum type, const char *data, int length, std::string &error) {
    GLuint shader;
    shader = glCreateShader(type);
    // Load shader source code
    glShaderSource(shader, 1, &data, &length);
    // Compile code
    glCompileShader(shader);
    // Check for failure
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char *log = new char[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        error += "etm::shader::Shader::compileShader(";
        error += std::to_string(type);
        error += ")::shader::COMPILATION_FAILED:\n";
        error += log;
        delete[] log;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint linkShaders(GLuint vertObject, GLuint fragObject, std::string &error) {
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertObject);
    glAttachShader(shaderProgram, fragObject);
    glLinkProgram(shaderProgram);
    // Check for failure
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char *log = new char[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        error += "etm::shader::Shader::linketm::shader::Shader_PROGRAM::COMPILATION_FAILED:\n";
        error += log;
        delete[] log;
        glDeleteProgram(shaderProgram);
        return 0;
    }
    return shaderProgram;
}
