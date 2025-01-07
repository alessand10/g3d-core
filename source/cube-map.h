#pragma once

class CubeMap {
    public: 
    class IG3DRenderTarget* cubeMapRT;
    class IG3DLogicalRenderTarget* cubeMapLRT;

    void init(class IG3DResourceFactory* resourceFactory, class IG3DDevice* device, int width, int height);

    void destroy();
};