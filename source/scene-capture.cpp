#include "scene-capture.h"
#include "g3d-engine.h"
#include "world.h"
#include "components/camera-component.h"
#include "components/transform-component.h"
#include "g3d-resource.h"
#include "g3d-execution.h"

void SceneCapture::init(G3DEngine *engine, int width, int height, int mipLevels)
{
    this->engine = engine;
    G3DWorld* world = engine->getWorld();

    G3DImageInfo depthStencilInfo{
        .device = engine->getDevice(),
        .width = width,
        .height = height,
        .mipLevels = 1,
        .arrayLayers = 1,
        .format = G3DFormat::D24_UNORM_S8_UINT,
        .isStaging = false,
        .usageBitmap = G3DImageUsage::DEPTH_STENCIL_RENDER_TARGET
    };

    depthStencilImage = engine->getResourceFactory()->createImage(&depthStencilInfo);

    depthStencilRT = engine->getResourceFactory()->createRenderTarget(depthStencilImage, true, 0);

    depthStencilLRT = engine->getResourceFactory()->createLogicalRenderTarget(depthStencilRT, false);

    // Create the entity for this scene capture
    entity = world->spawnEntity();

    const float pi = 3.14159265359f;

    // Create the camera component for this scene capture
    cameraComponentId = world->addComponent<CLASS_HASH(G3DCameraComponent)>(entity);
    G3DCameraComponent* cameraComponent = world->getComponentByComponentId<CLASS_HASH(G3DCameraComponent)>(cameraComponentId);
    cameraComponent->setVFOV(pi / 2.f); // 90 degree vertical field of view
    cameraComponent->setAspectRatio(1.f); // 1:1 aspect ratio, also gives 90 degree horizontal field of view
    cameraComponent->setNearPlane(0.1f);
    cameraComponent->setFarPlane(100.f); // Can be adjusted as needed

    // Create the world transform components for each face of the cube map
    for (int i = 0; i < 6; i++) {
        transformComponentIds[i] = world->addComponent<CLASS_HASH(G3DTransformComponent)>(entity);
    }

    G3DTransformComponent* transformComponent[6] = {
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[0]),
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[1]),
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[2]),
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[3]),
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[4]),
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[5])
    };

    // The default orientation of the camera is looking down the positive Z axis
    // Rotate the transform components to face the correct direction
    transformComponent[0]->rotate(pi/2.f, glm::vec3(0.f, 1.f, 0.f)); // Positive X
    transformComponent[1]->rotate(-(pi/2.f), glm::vec3(0.f, 1.f, 0.f)); // Negative X
    transformComponent[2]->rotate((pi/2.f), glm::vec3(1.f, 0.f, 0.f)); // Positive Y
    transformComponent[3]->rotate(-(pi/2.f), glm::vec3(1.f, 0.f, 0.f)); // Negative Y
    transformComponent[4]->rotate((pi), glm::vec3(0.f, 1.f, 0.f)); // Negative Z
    // Positive Z does not need to be rotated, it is the default orientation

    // Create the image for the scene capture

    G3DImageInfo imageInfo {
        .device = engine->getDevice(),
        .width = width,
        .height = height,
        .mipLevels = mipLevels,
        .arrayLayers = 6,
        .format = G3DFormat::BRGA8_UNORM,
        .isStaging = false,
        .usageBitmap = G3DImageUsage::SHADER_SAMPLING | G3DImageUsage::COLOR_RENDER_TARGET,
        .isCubemap = true
    };

    G3DImageViewInfo viewInfo {
        .baseMipLevel = 0,
        .mipLevels = mipLevels,
        .baseArrayLayer = 0,
        .arrayLayers = 6,
        .format = G3DFormat::BRGA8_UNORM,
        .aspectBitmap = G3DImageAspect::COLOR
    };

    IG3DResourceFactory* resourceFactory = engine->getResourceFactory();
    IG3DExecutionFactory* executionFactory = engine->getExecutionFactory();
    commandList = engine->getUtilityCommandList();

    renderViewport = {
        .x = 0,
        .y = 0,
        .width = (float)width,
        .height = (float)height,
        .minDepth = 0.f,
        .maxDepth = 1.f
    };

    renderScissor = {
        .x = 0,
        .y = 0,
        .width = (uint32_t)width,
        .height = (uint32_t)height
    };

    for (int i = 0; i < 6; i++) {
        cubeMapRT[i] = resourceFactory->createRenderTarget(
            resourceFactory->createImage(&imageInfo),
            false,
            i
        );
    }

    cubeMapLRT = resourceFactory->createLogicalRenderTarget(nullptr, false);
}

void SceneCapture::translateSceneCapture(float x, float y, float z)
{
    G3DWorld* world = engine->getWorld();
    for (int i = 0; i < 6; i++) {
        world->getComponentByComponentId<CLASS_HASH(G3DTransformComponent)>(transformComponentIds[i])->translate(glm::vec3(x, y, z));
    }
}

IG3DLogicalRenderTarget *SceneCapture::getLogicalRenderTarget(int faceIndex)
{
    cubeMapLRT->setRenderTarget(cubeMapRT[faceIndex]);
    return cubeMapLRT;
}
