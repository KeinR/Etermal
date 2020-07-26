#include "Translate.h"

#include <glm/gtx/rotate_vector.hpp>

#include "../Resources.h"
#include "Model.h"
#include "RModel.h"

glm::mat4 etm::tsl::model(Resources *res, Model &m) {
    glm::mat4 model(1.0f);

    // Translate so that the x/y coords are in the middle
    // of the object, then convert to quads (OpenGL style).
    const float xPos = (m.x + m.width / 2) / res->getViewportWidth() * 2 - 1;
    const float yPos = ((m.y + m.height / 2) / res->getViewportHeight() * 2 - 1) * -1;
    model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));

    // Convert width and height to OpenGL-readable clamped floats, 0-1
    model = glm::scale(model, glm::vec3(m.width / res->getViewportWidth(), m.height / res->getViewportHeight(), 0.0f));

    return model;
}

glm::mat4 etm::tsl::rModel(Resources *res, RModel &m) {
    return glm::rotate(model(res, m), glm::radians(m.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
}
