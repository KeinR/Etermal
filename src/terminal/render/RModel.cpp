#include "RModel.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl.h"
#include "../shader/Shader.h"
#include "../Resources.h"
#include "Translate.h"

etm::RModel::RModel(): RModel(0.0f, 0.0f, 0.0f, 0.0f, 0.0f) {
}
etm::RModel::RModel(float x, float y, float width, float height, float rotation):
    Model(x, y, width, height), rotation(rotation) {
}

void etm::RModel::set(Resources *res) {
    glm::mat4 model = tsl::rModel(res, *this);
    // Set the model to the location given by the shader
    glUniformMatrix4fv(res->getShader().getModel(), 1, GL_FALSE, glm::value_ptr(model));
}
