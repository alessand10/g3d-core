#include "scene-capture.h"
#include "g3d-execution.h"
#include "glm/ext/matrix_transform.hpp"

void SceneCapture::init(IG3DResourceFactory *resourceFactory, IG3DDevice *device, int width, int height)
{
    cubeMap.init(resourceFactory, device, width, height);
}

glm::mat4 SceneCapture::getCaptureWorldMatrix(G3DSceneCaptureOrientation orientation)
{
    // 
    glm::mat4 worldMatrix = glm::identity<glm::mat4>();
    switch (orientation)
    {
    case G3DSceneCaptureOrientation::POSITIVE_X:
        worldMatrix = glm::rotate(worldMatrix, glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        break;
    case G3DSceneCaptureOrientation::NEGATIVE_X:
        worldMatrix = glm::rotate(worldMatrix, glm::radians(-90.f), glm::vec3(0.f, 1.f, 0.f));
        break;
    case G3DSceneCaptureOrientation::POSITIVE_Y:
        worldMatrix = glm::rotate(worldMatrix, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        break;
    case G3DSceneCaptureOrientation::NEGATIVE_Y:
        worldMatrix = glm::rotate(worldMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        break;
    case G3DSceneCaptureOrientation::POSITIVE_Z:
        break;
    case G3DSceneCaptureOrientation::NEGATIVE_Z:
        worldMatrix = glm::rotate(worldMatrix, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
        break;
    }
}
