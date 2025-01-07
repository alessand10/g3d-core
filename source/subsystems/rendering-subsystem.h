#pragma once
#include "allocators/pooled-allocator.h"
#include "components/renderable.h"

class G3DRenderingSubsystem {
    PooledAllocator<G3DRenderableComponent> m_renderableAllocator;
    G3DRenderableComponent* m_activeRenderableComponent;
    public:
    
    G3DRenderableComponent* createRenderableComponent() {
        return new (m_renderableAllocator.allocate()) G3DRenderableComponent();
    }

    void setActiveRenderableComponent(G3DRenderableComponent* renderable) {
        m_activeRenderableComponent = renderable;
    }

    G3DRenderableComponent* getActiveRenderableComponent() {
        return m_activeRenderableComponent;
    }

    void drawActiveRenderableComponent() {
        
    }
};