#include "rendering-subsystem.h"

// This is safe to do, since all subsystems are singletons
G3DRenderingSubsystem* renderingSubsystemRef = nullptr;

void G3DRenderingSubsystem::init(G3DEngine* engine) {
    this->engine = engine;
    renderingSubsystemRef = this;
    m_renderableAllocator.init(maxRenderableComponents, sizeof(G3DRenderableComponent));
}

FGActionNode* G3DRenderingSubsystem::setActiveRenderableComponentNode(G3DFrameGraph* frameGraph) {
    // The active renderable is expected in the first input slot
    FGNodeID actionNodeId = frameGraph->addNode<FGActionNode>();

    FGActionNode* actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        G3DRenderableComponent* renderableComponent = node->getInputData<CLASS_HASH(G3DRenderableComponent)>(0);
        renderingSubsystemRef->setActiveRenderableComponent(renderableComponent);
    });

    return actionNode;
}

FGActionNode* G3DRenderingSubsystem::drawActiveRenderableComponentNode(G3DFrameGraph* frameGraph) {
    // The active renderable is expected in the first input slot
    FGNodeID actionNodeId = frameGraph->addNode<FGActionNode>();

    FGActionNode* actionNode = frameGraph->get<FGActionNode>(actionNodeId);

    actionNode->setAction([](FGActionNode* node) {
        renderingSubsystemRef->drawActiveRenderableComponent(node->getFrameGraph()->getCommandList());
    });

    return actionNode;
}