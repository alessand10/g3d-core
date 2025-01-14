#pragma once
#include "fg-node.h"
#include <algorithm>

class FGSequenceNode : public FGNode {
    GCLASS_BODY(FGSequenceNode)
    friend class G3DFrameGraph;
    FGNodeID* m_sequenceIDs = nullptr;
    unsigned char m_sequenceLength = 0U;
    unsigned char m_position = 0U;

    public:
    void init(int sequenceLength) {
        m_sequenceIDs = new FGNodeID[sequenceLength];
        m_sequenceLength = sequenceLength;

        std::fill(m_sequenceIDs, m_sequenceIDs + sequenceLength, -1);
    };

    void setNode(int index, FGNodeID identifier) {
        m_sequenceIDs[index] = identifier;
    }

    virtual void reset() {
        FGNode::reset();
        m_position = 0;
    }

    int execute(class G3DStackAllocator* nodeData) override { 
        if (m_done) return -1;
        if (m_position < m_sequenceLength) {
            while(m_sequenceIDs[m_position] == -1) {
                m_position++;
                if (m_position >= m_sequenceLength) {
                    m_done = true;
                    return -1;
                }
            }
            return m_sequenceIDs[m_position++];
        }
        else {
            m_done = true;
            return -1;
        } 
    };

};