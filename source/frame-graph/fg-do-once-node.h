#pragma once
#include "fg-node.h"

class FGDoOnceNode : public FGNode {
    GCLASS_BODY(FGDoOnceNode)
    friend class G3DFrameGraph;
    FGNodeID m_nextId = -1;
    public:
    void setNextNode(FGNodeID identifier) {
        m_nextId = identifier;
    }
    int execute(class G3DStackAllocator* nodeData) override { 
        if (m_done) return -1;
        m_done = true;
        return m_nextId; 
    };

};