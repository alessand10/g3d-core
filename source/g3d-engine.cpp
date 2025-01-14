#include "g3d-engine.h"
#include "g3d-api-layer.h"
#include "world.h"
#include "g3d-renderer.h"
#include "subsystems/pipeline/pipeline-subsystem.h"
#include "subsystems/geometry/geometry-subsystem.h"

void G3DEngine::start()
{
    m_systemFactory           = new G3DSystemFactoryVk();

    m_application             = m_systemFactory->createApplication();
    m_device                  = m_systemFactory->createDevice(m_application);

    m_executionFactory        = new G3DExecutionFactoryVk(m_device, m_application);
    m_resourceFactory         = new G3DResourceFactoryVk(m_device, m_application);
    m_renderPipelineFactory   = new G3DRenderPipelineFactoryVk(m_device);
    m_renderWindowFactory     = new G3DRenderWindowFactoryVk();

    m_world = new G3DWorld();

    m_pipelineSubsystem = new G3DPipelineSubsystem();
    m_pipelineSubsystem->init(this);

    m_renderer = new G3DRenderer();
    m_renderer->init(this);

    m_executor = m_executionFactory->createExecutor(G3DOperationType::GRAPHICS | G3DOperationType::TRANSFER);

    m_commandListAllocator = m_executionFactory->createCommandListAllocator(m_executor);

    m_geometrySubsystem = new G3DGeometrySubsystem();
    m_geometrySubsystem->init(this);

    createViewportWindow();
}

void G3DEngine::createViewportWindow() {
    m_renderWindow = m_renderWindowFactory->createRenderWindow(m_application, m_device, m_resourceFactory, m_windowHeight, m_windowWidth);
}


void G3DEngine::beginRenderLoop()
{
    while(!m_renderWindow->closeRequested()) {
        m_renderWindow->processEvents();
        m_renderer->render();
    }
    destroy();
}

void G3DEngine::destroy()
{
    m_renderer->destroy();
    m_world->destroy();
    m_renderWindow->destroy();
    m_commandListAllocator->destroy();
    m_geometrySubsystem->destroy();
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
