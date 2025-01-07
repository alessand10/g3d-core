#include "frame-graph.h"
#include "g3d-engine.h"

void G3DFrameGraph::init(G3DEngine* engine) {
    engine->registerGClass(FGDoOnceNode::GetGClass(), sizeof(FGDoOnceNode));
    engine->registerGClass(FGForLoopNode::GetGClass(), sizeof(FGForLoopNode));
    engine->registerGClass(FGActionNode::GetGClass(), sizeof(FGActionNode));

    m_nodeAllocator.init(MAX_NODES, 64U);
    m_nodeDataAllocator.init(MAX_NODES, 28U);
    m_entryNode = addDoOnceNode();
    m_exitNode = addDoOnceNode();
    m_entryNode->setNextNode(m_exitNode->getId());
}