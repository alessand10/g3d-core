#pragma once
#include "glm/mat4x4.hpp"

void scSetCamera(class G3DCameraComponent* camera, class G3DWorldTransformComponent* transform, glm::mat4* view, glm::mat4* proj);