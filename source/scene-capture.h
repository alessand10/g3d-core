#pragma once
#include "cube-map.h"
#include "glm/glm.hpp"

enum class G3DSceneCaptureOrientation {
    POSITIVE_X,
    NEGATIVE_X,
    POSITIVE_Y,
    NEGATIVE_Y,
    POSITIVE_Z,
    NEGATIVE_Z
};

class SceneCapture {
    protected:
    CubeMap cubeMap;
    public:
    void init(IG3DResourceFactory* resourceFactory, IG3DDevice* device, int width, int height);
    glm::mat4 getCaptureWorldMatrix(G3DSceneCaptureOrientation orientation);
};