#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl.h"
#include "../shader/Shader.h"
#include "../Resources.h"
#include "Translate.h"

etm::Model::Model(): Model(0.0f, 0.0f, 0.0f, 0.0f) {
}
etm::Model::Model(float x, float y, float width, float height):
    x(x), y(y), width(width), height(height) {
}

bool etm::Model::hasPoint(float x, float y) const {
    return this->x <= x && x <= this->x + width &&
            this->y <= y && y <= this->y + height;
}

void etm::Model::set(Resources *res) {
    glm::mat4 model = tsl::model(res, *this);
    // Set the model to the location given by the shader
    glUniformMatrix4fv(res->getShader().getModel(), 1, GL_FALSE, glm::value_ptr(model));
}
