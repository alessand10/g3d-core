#pragma once
#include "allocators/allocator-v2.h"
#include "components/renderable-component.h"
#include "g3d-engine.h"
#include "g3d-execution/g3d-command-list.h"
#include "frame-graph/fg-action-node.h"
#include "frame-graph/frame-graph.h"

class G3DRenderingSubsystem {
    G3DEngine* engine = nullptr;
    Allocator m_renderableAllocator;
    G3DRenderableComponent* m_activeRenderableComponent;

    const unsigned int maxRenderableComponents = 24U;

    public:

    void init(G3DEngine* engine);
    
    G3DRenderableComponent* createRenderableComponent() {
        return m_renderableAllocator.allocate<CLASS_HASH(G3DRenderableComponent)>();
    }

    void setActiveRenderableComponent(G3DRenderableComponent* renderable) {
        m_activeRenderableComponent = renderable;
    }

    G3DRenderableComponent* getActiveRenderableComponent() {
        return m_activeRenderableComponent;
    }

    void drawActiveRenderableComponent(IG3DCommandList* commandList) {
        // commandList->cmdDrawIndexed(
        //     m_activeRenderableComponent->getIndexCount(), 
        //     1U, 
        //     m_activeRenderableComponent->getIndexOffset(), 
        //     m_activeRenderableComponent->getVertexOffset(), 
        //     0U
        // );
    }

    FGActionNode* setActiveRenderableComponentNode(G3DFrameGraph* frameGraph);

    FGActionNode* drawActiveRenderableComponentNode(G3DFrameGraph* frameGraph);
};