#include "pbr-material.h"
#include "image.h"
#include "image-loader.h"
#include "g3d-engine.h"
#include "mipmap-generator.h"
#include "g3d-pipeline-config.h"
#include "g3d-resource.h"
#include "g3d-execution.h"
#include <vector>
#include <stdexcept>
#include "g3d-renderer.h"


void PBRMaterial::initSignature(G3DEngine *engine)
{
    IG3DPipelineFactory* pipelineFactory = engine->getPipelineFactory();
    if (resourceSetSignature == nullptr) {

        // The resource set signature representing all pbr materials
        std::vector<G3DPipelineBinding> pipelineBindings = {};

        uint32_t defaultPipelineBindingCount = engine->getRenderer()->getPipelineConfig()->pipelineBindingCount;
        G3DPipelineBinding* defaultPipelineBindings = engine->getRenderer()->getPipelineConfig()->pipelineBindings;
        // First add the default pipeline bindings
        for (int i = 0; i < defaultPipelineBindingCount; i++) {
            pipelineBindings.push_back(defaultPipelineBindings[i]);
        }

        // Then add material specific bindings
        for (int i = 0; i < pipelineBindingCount; i++) {
            pipelineBindings.push_back(PBRMaterial::pipelineBindings[i]);
        }

        // Support for 24 PBR materials
        resourceSetSignature = pipelineFactory->createResourceSetSignature(pipelineBindings.data(), pipelineBindings.size(), 24U);
    }
}

void PBRMaterial::init(G3DEngine *engine, Image *pbrTextures[4])
{
    IG3DPipelineFactory* pipelineFactory = engine->getPipelineFactory();
    initSignature(engine);


    this->width = pbrTextures[0]->getWidth();
    this->height = pbrTextures[0]->getHeight();

    // Ensure all images are the same size, otherwise throw an exception
    for (int i = 1; i < 4; i++) {
        if (pbrTextures[i]->getWidth() != width || pbrTextures[i]->getHeight() != height) {
            throw std::runtime_error("All images must be the same size");
        }
    }

    // Create a resource set instance for this material, now that we have verified that the images are suitable
    resourceSetInstance = pipelineFactory->createResourceSetInstance(resourceSetSignature);

    // The image info for all images
    G3DImageInfo imageInfo {
        .device = engine->getDevice(),
        .width = width,
        .height = height,
        .mipLevels = 1U,
        .arrayLayers = 1,
        .format = G3DFormat::RGBA8_UNORM,
        .isStaging = true,
        .usageBitmap = G3DImageUsage::MEMORY_TRANSFER_SOURCE | G3DImageUsage::MEMORY_TRANSFER_DESTINATION
    };

    IG3DResourceFactory* resourceFactory = engine->getResourceFactory();
    
    // Create a staging image to move the image into GPU memory
    IG3DImage* stagingImage = resourceFactory->createImage(&imageInfo);


    // Change the image info to create a gpu-optimized image
    imageInfo.isStaging = false;
    imageInfo.usageBitmap = G3DImageUsage::SHADER_SAMPLING | G3DImageUsage::MEMORY_TRANSFER_DESTINATION |
     G3DImageUsage::SHADER_WRITING;
    imageInfo.mipLevels = mipLevels;

    // Define the view info for each optimized image
    G3DImageViewInfo viewInfo {
        .baseMipLevel = 0,
        .mipLevels = mipLevels,
        .baseArrayLayer = 0,
        .arrayLayers = 1,
        .format = G3DFormat::RGBA8_UNORM,
        .aspectBitmap = G3DImageAspect::COLOR
    };


    IG3DCommandList* commandList = engine->getExecutionFactory()->allocateCommandList(engine->getCommandListAllocator());

    IG3DExecutor* executor = engine->getExecutor();
    for (int i = 0; i < 4; i++) {
        images[i] = resourceFactory->createImage(&imageInfo);
        // Load the image data into the staging image
        stagingImage->loadFromHostMemory(pbrTextures[i]->getData(), pbrTextures[i]->getByteSize());
        // Copy the image data to the gpu-optimized image

        // Wait until this command list is free for writing and execution
        commandList->waitUntilFreeAndReset();
        commandList->beginRecording();

        // Transition the image to a state where it can be copied to
        commandList->cmdEnsureSubImageReady(images[i], G3DImageUsage::MEMORY_TRANSFER_DESTINATION);

        // Copies the base mip level of the staging image to the base mip level of the gpu-optimized image
        commandList->cmdImageCopy(stagingImage, images[i]);

        // Transition the image to a state where it can be sampled in shaders
        commandList->cmdEnsureSubImageReady(images[i], G3DImageUsage::SHADER_SAMPLING);

        commandList->endRecording();

        executor->executeCommandList(commandList);

        imageViews[i] = resourceFactory->createImageView(images[i], &viewInfo);
        commandList->waitUntilFree();
    }


    std::vector<G3DBindingUpdate*> bindingUpdates = {};

    uint32_t defaultBindingCount = engine->getRenderer()->getPipelineConfig()->defaultBindingUpdateCount;
    G3DBindingUpdate** defaultBindingUpdates = engine->getRenderer()->getPipelineConfig()->defaultBindingUpdates;

    for (int i = 0; i < defaultBindingCount; i++) {
        bindingUpdates.push_back(defaultBindingUpdates[i]);
    }

    // Add the binding updates for the pbr textures
    G3DImageViewBindingUpdate imageViewBindingUpdate[4];

    for (int i = 0; i < 4; i++) {
        imageViewBindingUpdate[i].binding = i + defaultBindingCount;
        imageViewBindingUpdate[i].resource = imageViews[i];
        imageViewBindingUpdate[i].readWriteType = G3DImageAccessType::SAMPLE;

        bindingUpdates.push_back(&imageViewBindingUpdate[i]);
    }   

    resourceSetInstance->updateBindings(bindingUpdates.data(), bindingUpdates.size());

    MipmapGenerator::generateMipmaps(images[0]);
    MipmapGenerator::generateMipmaps(images[1]);
}
