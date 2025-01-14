#pragma once
#include "fg-node.h"

class FGBinarySequenceNode : public FGNode {
    GCLASS_BODY(FGBinarySequenceNode)
    friend class G3DFrameGraph;
    bool m_firstPathComplete = false;
    FGNodeID m_firstPathId = -1;
    FGNodeID m_secondPathId = -1;

    public:

    void setFirstPathNode(FGNodeID identifier) {
        m_firstPathId = identifier;
    }

    void setSecondPathNode(FGNodeID identifier) {
        m_secondPathId = identifier;
    }

    virtual void reset() {
        FGNode::reset();
        m_firstPathComplete = false;
    }

    virtual int execute(class G3DStackAllocator* nodeData) override { 
        if (m_done) return -1;
        if (!m_firstPathComplete) {
            m_firstPathComplete = true;
            return m_firstPathId;
        }
        else {
            m_done = true;
            return m_secondPathId;
        }
    };

};