#pragma once 
#include "fg-node.h"

class FGActionNode : public FGNode {
    GCLASS_BODY(FGActionNode)
    friend class G3DFrameGraph;
    public:
    void (*action)(FGActionNode* node);
    virtual void execute(class Allocator* nodeData) override { 
        action(this);
    };
};