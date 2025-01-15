#include "g3d-engine.h"
#include "g3d-api-layer.h"
#include "world.h"
#include "g3d-renderer.h"
#include "subsystems/pipeline/pipeline-subsystem.h"
#include "subsystems/geometry/geometry-subsystem.h"
#include "subsystems/material/material-subsystem.h"
#include "mipmap-generator.h"
#include "g3d-input.h"

void G3DEngine::start()
{
    m_systemFactory           = new G3DSystemFactoryVk();

    m_application             = m_systemFactory->createApplication();
    m_device                  = m_systemFactory->createDevice(m_application);

    m_executionFactory        = new G3DExecutionFactoryVk(m_device, m_application);
    m_resourceFactory         = new G3DResourceFactoryVk(m_device, m_application);
    m_renderPipelineFactory   = new G3DRenderPipelineFactoryVk(m_device);
    m_renderWindowFactory     = new G3DRenderWindowFactoryVk();

    m_executor = m_executionFactory->createExecutor(G3DOperationType::GRAPHICS | G3DOperationType::TRANSFER);

    m_commandListAllocator = m_executionFactory->createCommandListAllocator(m_executor);
    m_utilityCommandList = m_executionFactory->allocateCommandList(m_commandListAllocator);

    m_world = new G3DWorld();

    m_pipelineSubsystem = new G3DPipelineSubsystem();
    m_pipelineSubsystem->init(this);

    m_renderer = new G3DRenderer();
    m_renderer->init(this);


    m_geometrySubsystem = new G3DGeometrySubsystem();
    m_geometrySubsystem->init(this);

    m_materialSubsystem = new G3DMaterialSubsystem();
    m_materialSubsystem->init(this);

    MipmapGenerator::init(this);

    m_renderWindow = m_renderWindowFactory->createRenderWindow(m_application, m_device, m_resourceFactory, 1080U, 1920U);
    m_renderWindow->setCursorPositionCallback(cursorPositionChangeCallback);
    m_renderWindow->setKeyCallback(keyCallback);
    m_renderWindow->setMouseButtonCallback(mouseButtonCallback);
}

void G3DEngine::beginRenderLoop()
{

    uint32_t hashIds [] = {
        STRING_HASH(G3DCameraComponent),
        STRING_HASH(G3DTransformComponent)
    };


    EntityID* cameraEntites = nullptr;
    int numCameraEntities = 0;

    // Retrieve all entities that have the necessary components
    m_world->getAllEntitiesOfType(hashIds, 2, &cameraEntites, &numCameraEntities);
    G3DTransformComponent* camTransform = m_world->getComponentByEntity<CLASS_HASH(G3DTransformComponent)>(cameraEntites[0]);
    G3DCameraComponent* cam = m_world->getComponentByEntity<CLASS_HASH(G3DCameraComponent)>(cameraEntites[0]);
    initInputComponent(this, camTransform, cam);

    int frameNumber = 0;
    IG3DLogicalRenderTarget* windowColorRT = nullptr;
    IG3DLogicalRenderTarget* depthStencilRT = nullptr;
    while(!m_renderWindow->closeRequested()) {
        m_renderWindow->processEvents();
        tick();
        windowColorRT = m_renderWindow->getNextAvailableFrame(&frameNumber);
        depthStencilRT = m_renderWindow->getLogicalDepthStencilTarget();
        m_renderer->render(frameNumber, windowColorRT, depthStencilRT);
        m_renderWindow->presentOnRenderFinish(frameNumber);
    }
    destroy();
}

void G3DEngine::captureDeltaTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    deltaTimeMS = std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
    lastTime = currentTime;
}

void G3DEngine::tick()
{
    captureDeltaTime();
    tickCamera(deltaTimeMS);
}

void G3DEngine::destroy()
{
    m_renderer->destroy();
    m_world->destroy();
    m_renderWindow->destroy();
    m_commandListAllocator->destroy();
    m_geometrySubsystem->destroy();
    m_materialSubsystem->destroy();

    m_device->destroy();
    m_application->destroy();
    
    delete m_pipelineSubsystem;
    delete m_geometrySubsystem;
    delete m_renderer;

    delete m_systemFactory;
    delete m_executionFactory;
    delete m_resourceFactory;
    delete m_renderPipelineFactory;
    delete m_renderWindowFactory;
}
