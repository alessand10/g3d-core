#pragma once
#include "component-base.h"
#include "glm/mat4x4.hpp"

class G3DCameraComponent : public IG3DComponent {
    COMPONENT_BASE(G3DCameraComponent);

    float vFOV = 3.14f / 4.0f; // Default to 45 degree field of view
    float aspectRatio = 1920.f / 1080.f; // Default to 16:9 aspect ratio
    float nearPlane = 0.1f;
    float farPlane = 100.f;

    public:
    void init() override {};
    void update() override {};
    void destroy() override {};

    float getVFOV() {
        return vFOV;
    };

    float getAspectRatio() {
        return aspectRatio;
    };

    float getNearPlane() {
        return nearPlane;
    };

    float getFarPlane() {
        return farPlane;
    };
};