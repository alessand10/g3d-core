#pragma once
#include "fg-node.h"

class FGDoOnceNode : public FGNode {
    GCLASS_BODY(FGDoOnceNode)
    friend class G3DFrameGraph;
    public:
    void execute(class Allocator* nodeData) override {};

};