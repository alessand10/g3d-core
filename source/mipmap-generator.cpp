#include "mipmap-generator.h"
#include "g3d-defines.h"
#include "g3d-engine.h"
#include "g3d-resource.h"
#include "g3d-pipeline-config.h"
#include "g3d-execution.h"
#include <vector>
#include "file-loader.h"

void MipmapGenerator::init(G3DEngine *engine)
{
    
    G3DPipelineBinding computePipelineBindings[] {
        {
            .type = G3DPipelineBindingType::G3DImageViewRead,
            .shaderBitmap = G3DShader::G3DComputeShader,
        },
        {
            .type = G3DPipelineBindingType::G3DImageViewWrite,
            .shaderBitmap = G3DShader::G3DComputeShader,
        }
    };

    IG3DPipelineFactory* renderPipelineFactory = engine->getPipelineFactory();

    computeShaderRSet = renderPipelineFactory->createResourceSetSignature(computePipelineBindings, 2U, 1U);
    computeShaderRSetInstance = renderPipelineFactory->createResourceSetInstance(computeShaderRSet);

    char* computeShaderBytecode = nullptr;
    uint32_t computeShaderBytecodeSize = 0U;
    G3DFileLoader::Binary::loadFile("../shaders/build/mipmap.spv", &computeShaderBytecode, &computeShaderBytecodeSize);

    G3DComputePipelineInfo computePipelineInfo {
        .device = engine->getDevice(),
        .bytecode = computeShaderBytecode,
        .bytecodeSize = computeShaderBytecodeSize
    };

    IG3DResourceSetSignature* computeSupportedSignatures[] { computeShaderRSet };
    computePipeline = renderPipelineFactory->createComputePipeline(&computePipelineInfo, computeSupportedSignatures, 1U);
}

void MipmapGenerator::generateMipmaps(IG3DImage *image)
{
    IG3DResourceFactory* resourceFactory = engine->getResourceFactory();
    int mipLevels = image->getMipLevels();


    IG3DCommandList* commandList = engine->getUtilityCommandList();

    int width = image->getWidth();
    int height = image->getHeight();
    for (int baseMip = 0 ; baseMip < mipLevels - 1 ; baseMip++) {

        commandList->waitUntilFreeAndReset();
        commandList->reset();

        commandList->beginRecording();

        commandList->cmdImageTransition(image, G3DImageTransition::UNUSED_TO_COMPUTE_READ, 0, baseMip);
        commandList->cmdImageTransition(image, G3DImageTransition::UNUSED_TO_COMPUTE_WRITE, 0, baseMip + 1);

        commandList->endRecording();

        engine->getExecutor()->executeCommandList(commandList);

        G3DImageViewInfo viewInfo {
            .baseMipLevel = baseMip,
            .mipLevels = 1,
            .baseArrayLayer = 0,
            .arrayLayers = 1,
            .format = G3DFormat::RGBA8_UNORM,
            .aspectBitmap = G3DImageAspect::COLOR
        };

        IG3DImageView* baseMipView = resourceFactory->createImageView(image, &viewInfo);

        viewInfo.baseMipLevel = baseMip + 1;

        IG3DImageView* targetMipView = resourceFactory->createImageView(image, &viewInfo);

        G3DImageViewBindingUpdate sourceViewBindingUpdate;
        sourceViewBindingUpdate.binding = 0;
        sourceViewBindingUpdate.resource = baseMipView;
        sourceViewBindingUpdate.readWriteType = G3DImageAccessType::READ;

        G3DImageViewBindingUpdate targetViewBindingUpdate;
        targetViewBindingUpdate.binding = 1;
        targetViewBindingUpdate.resource = targetMipView;
        targetViewBindingUpdate.readWriteType = G3DImageAccessType::WRITE;

        G3DBindingUpdate* computeBindingUpdates[] = { &sourceViewBindingUpdate, &targetViewBindingUpdate};


        commandList->waitUntilFreeAndReset();
        commandList->reset();

        // Needs to wait for previous command lists to be done
        computeShaderRSetInstance->updateBindings(computeBindingUpdates, 2);

        commandList->beginRecording();

        commandList->cmdBindResourceSetInstance(computeShaderRSetInstance, computePipeline);
        commandList->cmdBindComputePipeline(computePipeline);
        
        width /= 2;
        height /= 2;

        commandList->cmdDispatch(width / 16, height / 16, 1);

        commandList->cmdImageTransition(image, G3DImageTransition::COMPUTE_WRITE_TO_FRAGMENT_READ, 0, baseMip);

        if (baseMip == mipLevels - 2) {
            commandList->cmdImageTransition(image, G3DImageTransition::COMPUTE_WRITE_TO_FRAGMENT_READ, 0, baseMip + 1);
        }

        commandList->endRecording();

        engine->getExecutor()->executeCommandList(commandList);

        commandList->waitUntilFree();

    }
}
