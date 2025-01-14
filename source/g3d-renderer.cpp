#include "g3d-renderer.h"
#include "g3d-defines.h"
#include "glm/mat4x4.hpp"
#include "g3d-resource.h"
#include "g3d-engine.h"
#include "world.h"
#include "g3d-execution/g3d-command-list.h"
#include "subsystems/pipeline/pipeline-subsystem.h"
#include "g3d-engine.h"
#include "g3d-pipeline-config.h"
#include "pbr-material.h"
#include "file-loader.h"
#include "subsystems/geometry/geometry-subsystem.h"

struct WorldCB {
    glm::mat4 world;
    float uvScaleX;
    float uvScaleY;
    // Pad to the next 64 byte boundary
    float padding[14];
};

struct ViewProjCB {
    glm::mat4 view;
    glm::mat4 projection;
};

// These are basic pipeline configs that won't change for any created pipeline
G3DVertexInput vertexInputs[] { 
        G3DVertexInput { // Position
            .shaderLocation = 0,
            .format = G3DFormat::RGB32_FLOAT
        },
        G3DVertexInput { // Normal
            .shaderLocation = 1,
            .format = G3DFormat::RGB32_FLOAT
        },
        G3DVertexInput { // TexCoord
            .shaderLocation = 2,
            .format = G3DFormat::RG32_FLOAT
        },
        G3DVertexInput { // Tangent
            .shaderLocation = 3,
            .format = G3DFormat::RGB32_FLOAT
        }
    };

G3DVertexBindingInputs perVertexAttributes {
    .vertexInputs = vertexInputs,
    .inputCount = 4,
    .bindingIndex = 0,
    .inputRate = G3DVertexInputRate::PER_VERTEX,
};

void G3DRenderer::init(G3DEngine *engine)
{   
    G3DPipelineSubsystem* pipelineSubsystem = engine->getPipelineSubsystem();

    int defaultVertexBindingInputsId = pipelineSubsystem->addVertexBindingInputs(perVertexAttributes);

    rendererPipelineConfig.pipelineBindings = new G3DPipelineBinding[3];

    rendererPipelineConfig.pipelineBindings[0] = {
        .type = G3DPipelineBindingType::G3DConstantBufferDynamic,
        .shaderBitmap = G3DShader::G3DVertexShader
    };

    rendererPipelineConfig.pipelineBindings[1] = {
        .type = G3DPipelineBindingType::G3DConstantBufferDynamic,
        .shaderBitmap = G3DShader::G3DVertexShader
    };

    rendererPipelineConfig.pipelineBindings[2] = {
        .type = G3DPipelineBindingType::G3DSampler,
        .shaderBitmap = G3DShader::G3DFragmentShader
    };

    G3DBufferViewBindingUpdate* worldCBUpdate = new G3DBufferViewBindingUpdate();
    worldCBUpdate->binding = 0;
    worldCBUpdate->resource = worldConstBufferView;
    worldCBUpdate->isDynamic = true;
    worldCBUpdate->range = sizeof(WorldCB);
    worldCBUpdate->offset = 0;

    G3DBufferViewBindingUpdate* viewProjCBUpdate = new G3DBufferViewBindingUpdate();
    viewProjCBUpdate->binding = 1;
    viewProjCBUpdate->resource = viewProjConstBufferView;
    viewProjCBUpdate->isDynamic = true;
    viewProjCBUpdate->range = sizeof(ViewProjCB);

    G3DSamplerBindingUpdate* samplerUpdate = new G3DSamplerBindingUpdate();
    samplerUpdate->binding = 2;
    samplerUpdate->resource = sampler;

    rendererPipelineConfig.defaultBindingUpdates[0] = worldCBUpdate;
    rendererPipelineConfig.defaultBindingUpdates[1] = viewProjCBUpdate;
    rendererPipelineConfig.defaultBindingUpdates[2] = samplerUpdate;

    // Now we can use the gpu-optimized image in our shaders
    char* vShaderByteCode = nullptr;
    char* fShaderByteCode = nullptr;
    uint32_t vShaderSize = 0;
    uint32_t fShaderSize = 0;
    G3DFileLoader::Binary::loadFile("../shaders/build/vshader.spv", &vShaderByteCode, &vShaderSize);
    G3DFileLoader::Binary::loadFile("../shaders/build/fshader.spv", &fShaderByteCode, &fShaderSize);

    G3DPipelineStageVertex vertexStage {
        .bytecode = vShaderByteCode,
        .bytecodeSize = vShaderSize,
        .vertexBindings = &perVertexAttributes,
        .vertexBindingCount = 1
    };
    int vertexStageId = pipelineSubsystem->addVertexStage(vertexStage);

    G3DPipelineStageFragment fragmentStage {
        .bytecode = fShaderByteCode,
        .bytecodeSize = fShaderSize
    };
    int fragmentStageId = pipelineSubsystem->addFragmentStage(fragmentStage);

    G3DPipelineState pipelineState {
        .depthTestEnabled = true,
        .depthWriteEnabled = true
    };
    int pipelineStateId = pipelineSubsystem->addPipelineState(pipelineState);

    PBRMaterial::initSignature(engine);
    int resourceSetSignatureId = pipelineSubsystem->addResourceSetSignature(PBRMaterial::getResourceSetSignature());

    G3DPipelineConfiguration pipelineConfig {
        .depthTestEnabled = true,
        .depthWriteEnabled = true,
        .vertexStageId = vertexStageId,
        .fragmentStageId = fragmentStageId,
        .pipelineStateId = pipelineStateId,
        .vertexBindingInputId = defaultVertexBindingInputsId,
        .resourceSetSignatureId = resourceSetSignatureId
    };

    IG3DRenderPipeline* renderPipeline = pipelineSubsystem->getPipelineForConfiguration(&pipelineConfig);
}

void G3DRenderer::render()
{

}

/**
 * @brief Render the scene using the provided render configuration
 */
void G3DRenderer::renderScene(G3DRenderConfig* renderConfig)
{
    IG3DCommandList* commandList = renderConfig->m_commandList;

    commandList->beginRecording();

    commandList->cmdSetViewport(*renderConfig->m_viewport);
    commandList->cmdSetScissor(*renderConfig->m_scissor);

    // Begin rendering to the provided targets
    commandList->cmdBeginRendering(&renderConfig->m_colorRT, 1U, renderConfig->m_depthStencilRT);

    G3DPipelineSubsystem* pipelineSubsystem = engine->getPipelineSubsystem();
    
    //commandList->cmdBindRenderPipeline(pipelineSubsystem->getPipelineForConfiguration());

    G3DWorld* world = engine->getWorld();

    // This will be populated with the list of entities that are renderable
    std::vector<G3DMeshInsertion> meshInsertions = engine->getGeometrySubsystem()->getInsertedMeshes();

    
}

void G3DRenderer::destroy()
{
    delete rendererPipelineConfig.defaultBindingUpdates[0];
    delete rendererPipelineConfig.defaultBindingUpdates[1];
    delete rendererPipelineConfig.defaultBindingUpdates[2];

    delete[] rendererPipelineConfig.pipelineBindings;
}
