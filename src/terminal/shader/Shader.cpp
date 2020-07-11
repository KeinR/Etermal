#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>

#include "../render/glfw.h"
#include "../util/error.h"

static char *loadFile(const std::string &path, int &length, std::string &error);
// Compiles the given shader and returns a handle to the OpenGL object.
// Throws an instance of gl_error if failed
static GLuint compileShader(GLenum type, const char *data, int length, std::string &error);
// Links two shaders (vertex and fragment) and returns the resulting
// shader program.
// Throws an instance of gl_error if failed
static GLuint linkShaders(GLuint vertObject, GLuint fragObject, std::string &error);

etm::shader::Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    std::string error;
    int vertLength;
    char *vertData = loadFile(vertexPath, vertLength, error);
    if (error.size()) {
        throw fe_error(error + " @" + vertexPath);
    }
    int fragLength;
    char *fragData = loadFile(fragmentPath, fragLength, error);
    if (error.size()) {
        delete[] vertData;
        throw fe_error(error + " @" + fragmentPath);
    }
    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertData, vertLength, error);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragData, fragLength, error);
    if (error.size()) {
        glDeleteShader(vertShader);
        throw fe_error(error);
    }
    shader = linkShaders(vertShader, fragShader, error);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    if (error.size()) {
        throw fe_error(error);
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

char *loadFile(const std::string &path, int &length, std::string &error) {
    std::ifstream file(path);
    if (!file.good()) {
        error = "etm::shader::Shader::loadFile: Failed to open shader file";
        return nullptr;
    }
    file.seekg(0, file.end);
    length = file.tellg();
    file.seekg(0, file.beg);

    char *data = new char[length];

    file.read(data, length);

    if (!file.good()) {
        error = "etm::shader::Shader::loadFile: Failed to read from shader file";
        delete[] data;
        return nullptr;
    }

    file.close();

    return data;
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
