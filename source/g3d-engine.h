#pragma once
#include "glm/glm.hpp"
#include "gclass.h"

struct ConstantBuffer {
    glm::mat4 world;
    glm::mat4 view;
    glm::mat4 projection;
};

class G3DEngine {

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

    /**
     * The 3D world that the engine is rendering.
     */
    class G3DWorld* m_world;


    public:
    void start();
    void stop();
    G3DWorld* getWorld() {
        return m_world;
    }

    template <typename T>
    T* getSubsystem() {
        
    }

    static inline int m_classId = 0;
    void registerGClass(GClassProperties* gclass, size_t size) {
        //gclass->setClassID(m_classId++);
        gclass->setSize(size);
    }
};