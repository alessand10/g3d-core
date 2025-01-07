#include "g3d-engine.h"
#include "g3d-api-layer.h"
#include "world.h"

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
}

void G3DEngine::stop()
{
    delete m_world;
    m_device->destroy();
    m_application->destroy();
    
    delete m_systemFactory;
    delete m_executionFactory;
    delete m_resourceFactory;
    delete m_renderPipelineFactory;
    delete m_renderWindowFactory;
}
