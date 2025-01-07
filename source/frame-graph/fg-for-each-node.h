#pragma once
#include "fg-node.h"


class FGForLoopNode : public FGNode {
    GCLASS_BODY(FGForLoopNode)
    friend class G3DFrameGraph;

    FGNodeID repeatNodeID;

    bool m_pendingReset = false;

    public:

    void setRepeatNode(FGNodeID identifier) {
        repeatNodeID = identifier;
    }

    FGNodeID getRepeatNode() {
        return repeatNodeID;
    }

    void reset() {
        (*getOutputData<CLASS_HASH(int)>(0)) = -1;
        m_pendingReset = false;
    }

    void execute(class Allocator* nodeData) override {
        int* iterationVariable = getOutputData<CLASS_HASH(int)>(0);
        int* numIterations = getOutputData<CLASS_HASH(int)>(1);
        if (*iterationVariable < *numIterations) 
            (*iterationVariable)++;
        
        if (*iterationVariable == *numIterations) 
            m_pendingReset = true;
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