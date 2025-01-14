#pragma once
#include "g3d-defines.h"

class SceneCapture {
    class G3DEngine* engine = nullptr;
    // Create an entity for the scene capture
    int entity = -1;

    // We have 6 world transform components, one for each face of the cube map
    int transformComponentIds[6] = {};

    // One camera component for the scene capture
    int cameraComponentId = -1;

    class IG3DImage* sceneCaptureImage = nullptr;

    class IG3DImage* depthStencilImage = nullptr;

    class IG3DRenderTarget* depthStencilRT = nullptr;

    class IG3DLogicalRenderTarget* depthStencilLRT = nullptr;

    // The cube map render target is one image with 6 array layers
    class IG3DRenderTarget* cubeMapRT[6];

    // The logical render target for each face of the cube map
    class IG3DLogicalRenderTarget* cubeMapLRT;

    class IG3DCommandList* commandList = nullptr;

    G3DViewport renderViewport;

    G3DScissor renderScissor;

    public:
    G3DViewport getRenderViewport() { return renderViewport; }
    G3DScissor getRenderScissor() { return renderScissor; }

    void init(class G3DEngine* engine, int width, int height, int mipLevels);

    // In most cases, the scene capture will not need to be moved
    void translateSceneCapture(float x, float y, float z);

    class IG3DCommandList* getCommandList() { return commandList; }

    class IG3DLogicalRenderTarget* getLogicalRenderTarget(int faceIndex);

    class IG3DLogicalRenderTarget* getLogicalDepthStencilTarget() { return depthStencilLRT; }

    int getCameraComponentId() { return cameraComponentId; }

    int getTransformComponentId(int faceIndex) { return transformComponentIds[faceIndex]; }

};