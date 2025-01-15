#pragma once
#include "glm/glm.hpp"
#include "gclass.h"
#include <chrono>

struct ConstantBuffer {
    glm::mat4 world;
    glm::mat4 view;
    glm::mat4 projection;
};

class G3DEngine {

    double deltaTimeMS = 0.0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
    void captureDeltaTime();

    /**
     * The set of factories used by subsystems of the G3DEngine.
     */
    class IG3DSystemFactory* m_systemFactory;
    class IG3DResourceFactory* m_resourceFactory;
    class IG3DExecutionFactory* m_executionFactory;
    class IG3DPipelineFactory* m_renderPipelineFactory;
    class IG3DRenderWindowFactory* m_renderWindowFactory;

    /**
     * The core interfaces used by subsystems of the G3DEngine.
     */
    class IG3DDevice* m_device;
    class IG3DApplication* m_application;
    class IG3DRenderWindow* m_renderWindow;

    class IG3DBuffer* m_constantBuffer;
    class G3DGeometricPartition* m_activePartition;
    class IG3DCommandListAllocator* m_commandListAllocator;
    class IG3DCommandList* m_utilityCommandList;

    class IG3DExecutor* m_executor;

    class G3DPipelineSubsystem* m_pipelineSubsystem;
    class G3DGeometrySubsystem* m_geometrySubsystem;
    class G3DMaterialSubsystem* m_materialSubsystem;

    /**
     * The 3D world that the engine is rendering.
     */
    class G3DWorld* m_world;

    class G3DRenderer* m_renderer;
    void tick();

    void destroy();

    public:
    class G3DPipelineSubsystem* getPipelineSubsystem() {
        return m_pipelineSubsystem;
    }

    class G3DGeometrySubsystem* getGeometrySubsystem() {
        return m_geometrySubsystem;
    }

    class G3DMaterialSubsystem* getMaterialSubsystem() {
        return m_materialSubsystem;
    }

    IG3DSystemFactory* getSystemFactory() {
        return m_systemFactory;
    }

    IG3DResourceFactory* getResourceFactory() {
        return m_resourceFactory;
    }

    IG3DExecutionFactory* getExecutionFactory() {
        return m_executionFactory;
    }

    IG3DPipelineFactory* getPipelineFactory() {
        return m_renderPipelineFactory;
    }

    IG3DRenderWindowFactory* getRenderWindowFactory() {
        return m_renderWindowFactory;
    }

    IG3DExecutor* getExecutor() {
        return m_executor;
    }

    IG3DDevice* getDevice() {
        return m_device;
    }

    IG3DApplication* getApplication() {
        return m_application;
    }

    G3DRenderer* getRenderer() {
        return m_renderer;
    }

    IG3DRenderWindow* getRenderWindow() {
        return m_renderWindow;
    }

    void start();

    void beginRenderLoop();


    G3DWorld* getWorld() {
        return m_world;
    }

    template <typename T>
    T* getSubsystem() {
        
    }

    class IG3DCommandListAllocator* getCommandListAllocator() {
        return m_commandListAllocator;
    }

    class IG3DCommandList* getUtilityCommandList() {
        return m_utilityCommandList;
    }

    static inline int m_classId = 0;
    void registerGClass(GClassProperties* gclass, size_t size) {
        //gclass->setClassID(m_classId++);
        gclass->setSize(size);
    }
};