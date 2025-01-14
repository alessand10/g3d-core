#pragma once 
#include "fg-node.h"

class FGActionNode : public FGNode {
    GCLASS_BODY(FGActionNode)
    friend class G3DFrameGraph;
    FGNodeID m_nextId = -1;
    public:

    void setNextNode(FGNodeID identifier) {
        m_nextId = identifier;
    }

    void setAction(void (*action)(FGActionNode* node)) {
        this->action = action;
    }

    void (*action)(FGActionNode* node);
    virtual int execute(class G3DStackAllocator* nodeData) override { 
        if (m_done) return -1;
        action(this);
        m_done = true;
        return m_nextId;
    };
};