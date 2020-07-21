#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl.h"
#include "../shader/Shader.h"

etm::Model::Model(): Model(0.0f, 0.0f, 0.0f, 0.0f) {
}
etm::Model::Model(float x, float y, float width, float height):
    x(x), y(y), width(width), height(height) {
}

bool etm::Model::hasPoint(float x, float y) const {
    return this->x <= x && x <= this->x + width &&
            this->y <= y && y <= this->y + height;
}

void etm::Model::set(const shader::Shader &shader) {
    glm::mat4 model(1.0f);

    // Translations are based off of the viewport, anyways
    // 0=x, 1=y, 2=width, 3=height
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Translate so that the x/y coords are in the middle
    // of the object, then convert to quads (OpenGL style).
    const float xPos = (x + width / 2) / viewport[2] * 2 - 1;
    const float yPos = ((y + height / 2) / viewport[3] * 2 - 1) * -1;
    model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));

    // Convert width and height to OpenGL-readable clamped floats, 0-1
    model = glm::scale(model, glm::vec3(width / viewport[2], height / viewport[3], 0.0f));

    // Set the model to the location given by the shader
    glUniformMatrix4fv(shader.getModel(), 1, GL_FALSE, glm::value_ptr(model));
}
