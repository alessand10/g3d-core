#include "fg-node.h"
#include "frame-graph.h"

G3DStackAllocator* FGNode::getNodeDataAllocator() {
    return m_frameGraph->getNodeDataAllocator();
}