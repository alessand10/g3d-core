#pragma once
#include "fg-node.h"


class FGForLoopNode : public FGNode {
    GCLASS_BODY(FGForLoopNode)
    friend class G3DFrameGraph;

    FGNodeID m_repeatId = -1;
    FGNodeID m_nextId = -1;

    bool m_pendingReset = false;

    public:

    void setRepeatNode(FGNodeID identifier) {
        m_repeatId = identifier;
    }

    FGNodeID getRepeatNode() {
        return m_repeatId;
    }

    virtual void reset() {
        FGNode::reset();
        (*getOutputData<CLASS_HASH(int)>(0)) = -1;
        m_pendingReset = false;
    }

    int execute(class G3DStackAllocator* nodeData) override {
        if (m_done) return -1;

        int* iterationVariable = getOutputData<CLASS_HASH(int)>(0);
        int* numIterations = getOutputData<CLASS_HASH(int)>(1);
        if (*iterationVariable < (*numIterations) - 1) {
            (*iterationVariable)++;
            return m_repeatId;
        }
        else {
            m_done = true;
            m_pendingReset = true;
            return m_nextId;
        }
    };

    int getCurrentIteration() {
        return (*getOutputData<CLASS_HASH(int)>(0));
    }

    int getIterationCount() {
        return (*getOutputData<CLASS_HASH(int)>(1));
    }

    int getForLoopDataId() {
        return outputDataIds[0];
    }

    bool getPendingReset() {
        return m_pendingReset;
    }

};