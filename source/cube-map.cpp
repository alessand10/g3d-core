#include "cube-map.h"
#include "g3d-resource.h"


void CubeMap::init(IG3DResourceFactory* resourceFactory, IG3DDevice* device, int width, int height)
{
    G3DImageInfo imageInfo {
        .device = device,
        .width = width,
        .height = height,
        .mipLevels = 1,
        .arrayLayers = 6,
        .format = G3DFormat::BRGA8_UNORM,
        .isStaging = false,
        .usageBitmap = G3DImageUsage::SHADER_SAMPLING | G3DImageUsage::COLOR_RENDER_TARGET
    };

    G3DImageViewInfo viewInfo {
        .baseMipLevel = 0,
        .mipLevels = 1,
        .baseArrayLayer = 0,
        .arrayLayers = 6,
        .format = G3DFormat::BRGA8_UNORM,
        .aspectBitmap = G3DImageAspect::COLOR
    };

    cubeMapRT = resourceFactory->createRenderTarget(
        resourceFactory->createImage(&imageInfo),
        resourceFactory->createImageView(resourceFactory->createImage(&imageInfo), &viewInfo)
    );

    cubeMapLRT = resourceFactory->createLogicalRenderTarget(cubeMapRT, 0);
}

void CubeMap::destroy()
{
    // cubeMapLRT->destroy();
    // cubeMapRT->destroy();
}
