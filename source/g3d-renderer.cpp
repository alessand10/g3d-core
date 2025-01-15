#include "g3d-renderer.h"
#include "g3d-defines.h"
#include "glm/mat4x4.hpp"
#include "g3d-resource.h"
#include "g3d-engine.h"
#include "world.h"
#include "g3d-execution/g3d-command-list.h"
#include "g3d-execution.h"
#include "subsystems/pipeline/pipeline-subsystem.h"
#include "g3d-engine.h"
#include "g3d-pipeline-config.h"
#include "pbr-material.h"
#include "file-loader.h"
#include "subsystems/geometry/geometry-subsystem.h"
#include "subsystems/material/material-subsystem.h"
#include "components/camera-component.h"
#include "components/renderable-component.h"
#include "components/transform-component.h"
#include "components/material-component.h"
#include "glm/ext.hpp"

struct PushConstants {
    glm::mat4 world;
    float uvScaleX;
    float uvScaleY;
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
    this->engine = engine;
    IG3DResourceFactory* resourceFactory = engine->getResourceFactory();
    IG3DExecutionFactory* executionFactory = engine->getExecutionFactory();
    IG3DCommandListAllocator* commandListAllocator = engine->getCommandListAllocator();

    for (int i = 0; i < 4; i++) {
        commandListPerFrame[i] = executionFactory->allocateCommandList(commandListAllocator);
    }

    // ------- Create the necessary pipeline resources for the renderer (uniform buffers) ---------

    G3DBufferInfo viewProjBufferInfo {
        .requestedSize = sizeof(ViewProjCB) * 7, // 1 for the main camera, 6 for the scene capture
        .isStaging = true,
        .usage = G3DBufferUsage::CONSTANT_BUFFER
    };

    viewProjConstBuffer = resourceFactory->createBuffer(&viewProjBufferInfo);

    G3DBufferViewInfo bufferViewInfo {
    };

    viewProjConstBufferView = resourceFactory->createBufferView(viewProjConstBuffer, &bufferViewInfo);

    G3DBufferInfo worldBufferInfo {
        .requestedSize = sizeof(PushConstants) * MAX_MESHES, // Support a world matrix for each mesh
        .isStaging = true,
        .usage = G3DBufferUsage::CONSTANT_BUFFER
    };

    worldConstBuffer = resourceFactory->createBuffer(&worldBufferInfo);

    G3DBufferViewInfo worldBufferViewInfo {
    };

    worldConstBufferView = resourceFactory->createBufferView(worldConstBuffer, &worldBufferViewInfo);

    worldConstBuffer->mapMemory(&mappedWorldConstBuffer, sizeof(PushConstants), 0U);
    viewProjConstBuffer->mapMemory(&mappedViewProjConstBuffer, sizeof(ViewProjCB) * 7, 0U);

    // ----------------------------------------------------------------------------------------------

    sampler = resourceFactory->createSampler();

    G3DPipelineSubsystem* pipelineSubsystem = engine->getPipelineSubsystem();

    int defaultVertexBindingInputsId = pipelineSubsystem->addVertexBindingInputs(perVertexAttributes);

    rendererPipelineConfig.pipelineBindings = new G3DPipelineBinding[3];

    rendererPipelineConfig.pipelineBindings[0] = {
        .type = G3DPipelineBindingType::G3DConstantBuffer,
        .shaderBitmap = G3DShader::G3DVertexShader
    };

    rendererPipelineConfig.pipelineBindings[1] = {
        .type = G3DPipelineBindingType::G3DConstantBuffer,
        .shaderBitmap = G3DShader::G3DVertexShader
    };

    rendererPipelineConfig.pipelineBindings[2] = {
        .type = G3DPipelineBindingType::G3DSampler,
        .shaderBitmap = G3DShader::G3DFragmentShader
    };

    G3DBufferViewBindingUpdate* worldCBUpdate = new G3DBufferViewBindingUpdate();
    worldCBUpdate->binding = 0;
    worldCBUpdate->resource = worldConstBufferView;
    worldCBUpdate->offset = 0;
    worldCBUpdate->range = sizeof(PushConstants);

    G3DBufferViewBindingUpdate* viewProjCBUpdate = new G3DBufferViewBindingUpdate();
    viewProjCBUpdate->binding = 1;
    viewProjCBUpdate->resource = viewProjConstBufferView;
    viewProjCBUpdate->offset = 0;
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

    G3DPipelineConstant constants[] {
        {
            .size = sizeof(PushConstants),
            .offset = 0,
            .shaderBitmap = G3DShader::G3DVertexShader
        },
    };

    G3DPipelineConstantSet constantSet {
        .constants = constants,
        .constantCount = 1
    };

    int pipelineConstantSetId = pipelineSubsystem->addPipelineConstantSet(constantSet);

    PBRMaterial::initSignature(engine);
    int resourceSetSignatureId = pipelineSubsystem->addResourceSetSignature(PBRMaterial::getResourceSetSignature());

    G3DPipelineConfiguration pipelineConfig {
        .depthTestEnabled = true,
        .depthWriteEnabled = true,
        .vertexStageId = vertexStageId,
        .fragmentStageId = fragmentStageId,
        .pipelineStateId = pipelineStateId,
        .vertexBindingInputId = defaultVertexBindingInputsId,
        .resourceSetSignatureId = resourceSetSignatureId,
        .pipelineConstantSetId = pipelineConstantSetId
    };

    IG3DRenderPipeline* renderPipeline = pipelineSubsystem->getPipelineForConfiguration(&pipelineConfig);
}

void G3DRenderer::render(int frameNumber, IG3DLogicalRenderTarget* screenColorRT, IG3DLogicalRenderTarget* screenDepthStencilRT)
{
    // This is the start of a render frame

    G3DViewport viewport = {
        .x = 0,
        .y = 0,
        .width = screenColorRT->getRenderTarget()->getRenderTargetImage()->getWidth(),
        .height = screenColorRT->getRenderTarget()->getRenderTargetImage()->getHeight(),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    G3DScissor scissor = {
        .x = 0,
        .y = 0,
        .width = screenColorRT->getRenderTarget()->getRenderTargetImage()->getWidth(),
        .height = screenColorRT->getRenderTarget()->getRenderTargetImage()->getHeight(),
    };

    G3DRenderConfig renderConfig;

    G3DWorld* world = engine->getWorld();

    renderConfig.m_commandList = commandListPerFrame[frameNumber];
    renderConfig.m_viewport = &viewport;
    renderConfig.m_scissor = &scissor;
    renderConfig.m_colorRT = screenColorRT;
    renderConfig.m_depthStencilRT = screenDepthStencilRT;


    /**
     * Determine the active camera entity
     */
    uint32_t hashIds [] = {
        STRING_HASH(G3DCameraComponent),
        STRING_HASH(G3DTransformComponent)
    };


    EntityID* cameraEntites = nullptr;
    int numCameraEntities = 0;

    // Retrieve all entities that have the necessary components
    world->getAllEntitiesOfType(hashIds, 2, &cameraEntites, &numCameraEntities);

    renderConfig.m_viewerEntityId = cameraEntites[0];

    commandListPerFrame[frameNumber]->waitUntilFreeAndReset();
    commandListPerFrame[frameNumber]->beginRecording();

    commandListPerFrame[frameNumber]->cmdEnsureSubImageReady(
        screenColorRT->getRenderTarget()->getRenderTargetImage(),
        G3DImageUsage::COLOR_RENDER_TARGET
    );

    renderScene(&renderConfig);

    commandListPerFrame[frameNumber]->cmdEnsureSubImageReady(
        screenColorRT->getRenderTarget()->getRenderTargetImage(),
        G3DImageUsage::PRESENTATION
    );

    commandListPerFrame[frameNumber]->endRecording();
    engine->getExecutor()->executeCommandList(commandListPerFrame[frameNumber]);
}

/**
 * @brief Render the scene using the provided render configuration
 */
void G3DRenderer::renderScene(G3DRenderConfig* renderConfig)
{
    G3DWorld* world = engine->getWorld();
    
    uint32_t hashIds [] = {
        STRING_HASH(G3DRenderableComponent),
        STRING_HASH(G3DTransformComponent),
        STRING_HASH(G3DMaterialComponent)
    };

    EntityID* meshEntities = nullptr;
    int numMeshEntities = 0;

    // Retrieve all entities that have the necessary components to be a mesh entity
    world->getAllEntitiesOfType(hashIds, 3, &meshEntities, &numMeshEntities);

    G3DCameraComponent* cam = world->getComponentByEntity<CLASS_HASH(G3DCameraComponent)>(renderConfig->m_viewerEntityId);
    G3DTransformComponent* camTransform = world->getComponentByEntity<CLASS_HASH(G3DTransformComponent)>(renderConfig->m_viewerEntityId);

    ViewProjCB viewProjCB {};

    glm::mat4 camTransformMatrix = camTransform->getTransformMatrix();

    glm::vec3 up = camTransform->getUpDirection();
    glm::vec3 forward = camTransform->getForwardDirection();
    glm::vec3 position = camTransform->getPosition();
    
    viewProjCB.view = glm::lookAtRH(
        position,
        position + forward,
        up
    );

    viewProjCB.projection = glm::perspectiveRH(
        cam->getVFOV(),
        cam->getAspectRatio(),
        cam->getNearPlane(),
        cam->getFarPlane()
    );

    memcpy(mappedViewProjConstBuffer, &viewProjCB, sizeof(ViewProjCB));

    IG3DCommandList* commandList = renderConfig->m_commandList;

    commandList->cmdSetViewport(*renderConfig->m_viewport);
    commandList->cmdSetScissor(*renderConfig->m_scissor);

    // Add this barrier to ensure that host writes occur before all stages of the pipeline
    commandList->cmdWaitForHostWrite();

    // Begin rendering to the provided targets
    commandList->cmdBeginRendering(&renderConfig->m_colorRT, 1U, renderConfig->m_depthStencilRT);

    G3DPipelineSubsystem* pipelineSubsystem = engine->getPipelineSubsystem();

    G3DPipelineConfiguration pipelineConfig;
    pipelineConfig.depthTestEnabled = true;
    pipelineConfig.depthWriteEnabled = true;
    pipelineConfig.vertexStageId = 0;
    pipelineConfig.fragmentStageId = 0;
    pipelineConfig.pipelineStateId = 0;
    pipelineConfig.vertexBindingInputId = 0;
    pipelineConfig.resourceSetSignatureId = 0;
    pipelineConfig.pipelineConstantSetId = 0;

    IG3DRenderPipeline* renderPipeline = pipelineSubsystem->getPipelineForConfiguration(&pipelineConfig);

    commandList->cmdBindRenderPipeline(renderPipeline);
    commandList->cmdBindVertexBuffer(engine->getGeometrySubsystem()->getVertexBuffer(), 0);
    commandList->cmdBindIndexBuffer(engine->getGeometrySubsystem()->getIndexBuffer());

    

    for (int i = 0; i < numMeshEntities; i++) {
        EntityID entityId = meshEntities[i];

        G3DTransformComponent* transformComponent = world->getComponentByEntity<CLASS_HASH(G3DTransformComponent)>(entityId);
        G3DMaterialComponent* materialComponent = world->getComponentByEntity<CLASS_HASH(G3DMaterialComponent)>(entityId);
        G3DRenderableComponent* renderableComponent = world->getComponentByEntity<CLASS_HASH(G3DRenderableComponent)>(entityId);

        PushConstants pushConstants;
        pushConstants.world = transformComponent->getTransformMatrix();
        pushConstants.uvScaleX = 1.0f;
        pushConstants.uvScaleY = 1.0f;

        commandList->cmdPushConstants(renderPipeline, G3DShader::G3DVertexShader, &pushConstants, sizeof(PushConstants), 0);

        engine->getMaterialSubsystem()->getMaterialResourceSetInstance(materialComponent->getMaterialId());
        commandList->cmdBindResourceSetInstance(
            engine->getMaterialSubsystem()->getMaterialResourceSetInstance(materialComponent->getMaterialId()),
            renderPipeline
        );

        G3DMeshInsertion* insertion = engine->getGeometrySubsystem()->getMeshInsertion(renderableComponent->getMeshID());

        G3DMesh* mesh = engine->getGeometrySubsystem()->getMesh(insertion->id);
        commandList->cmdDrawIndexed(
            mesh->getIndexCount(),
            1U,
            insertion->indexOffset,
            insertion->vertexOffset,
            0U
        );
    }

    commandList->cmdEndRendering();
}

void G3DRenderer::destroy()
{
    delete rendererPipelineConfig.defaultBindingUpdates[0];
    delete rendererPipelineConfig.defaultBindingUpdates[1];
    delete rendererPipelineConfig.defaultBindingUpdates[2];

    delete[] rendererPipelineConfig.pipelineBindings;
}
