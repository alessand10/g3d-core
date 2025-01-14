#include "scene-commands.h"
#include "components/camera-component.h"
#include "components/transform-component.h"
#include "glm/ext.hpp"

void scSetCamera(G3DCameraComponent *camera, G3DTransformComponent *transform, glm::mat4* view, glm::mat4* proj)
{
    *view = glm::lookAtRH(transform->getPosition(), transform->getPosition() + transform->getForwardDirection(), transform->getUpDirection());
    *proj = glm::perspective(camera->getVFOV(), camera->getAspectRatio(), camera->getNearPlane(), camera->getFarPlane());
    (*proj)[1][1] *= -1.0f;
}
