#pragma once
#include "component-base.h"

class G3DCubemapCaptureComponent : public IG3DComponent {
    COMPONENT_BASE(G3DCubemapCaptureComponent);

    const float vFOV = 3.14f / 2.0f; // Locked to 90 degree field of view
    const float aspectRatio = 1.f; // Locked to 1:1 aspect ratio


    float nearPlane = 0.1f;
    float farPlane = 100.f;

    // The render targets that this camera will render to
    class IG3DLogicalRenderTarget* colorRenderTargets[6]{};
    class IG3DLogicalRenderTarget* depthStencilRenderTarget = nullptr;

    public:
    void init() override {};
    void update() override {};
    void destroy() override {};

    void setLogicalColorRenderTarget(int faceIndex, class IG3DLogicalRenderTarget* renderTarget) {
        this->colorRenderTargets[faceIndex] = renderTarget;
    };

    void setLogicalDepthStencilRenderTarget(class IG3DLogicalRenderTarget* renderTarget) {
        this->depthStencilRenderTarget = renderTarget;
    };

    void setNearPlane(float nearPlane) {
        this->nearPlane = nearPlane;
    };

    void setFarPlane(float farPlane) {
        this->farPlane = farPlane;
    };

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