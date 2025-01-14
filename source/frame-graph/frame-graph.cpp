#include "frame-graph.h"
#include "g3d-engine.h"
#include "g3d-execution/g3d-executor.h"
#include "g3d-execution/g3d-execution-factory.h"

void G3DFrameGraph::init(G3DEngine* engine) {
    engine->registerGClass(FGDoOnceNode::GetGClass(), sizeof(FGDoOnceNode));
    engine->registerGClass(FGForLoopNode::GetGClass(), sizeof(FGForLoopNode));
    engine->registerGClass(FGActionNode::GetGClass(), sizeof(FGActionNode));

    // Init with 1024 bytes
    m_nodeFGData.init(1024U);

    m_commandListAllocator = engine->getExecutionFactory()->createCommandListAllocator(engine->getExecutor());
    m_commandList = engine->getExecutionFactory()->allocateCommandList(m_commandListAllocator);
}