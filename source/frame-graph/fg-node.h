#pragma once
#include "gclass.h"
#include "allocators/g3d-stack-allocator.h"

#define MAX_INPUTS 3
#define MAX_OUTPUTS 3

// Represents the node type in the frame graph
typedef int FGNodeID;

class FGNode : public GClass {
    public:
    FGNodeID m_id;
    bool m_done = false;

    class G3DFrameGraph* m_frameGraph = nullptr;

    // This is a general purpose data reference that can be used to store any type of data
    int16_t inputDataIds[MAX_INPUTS];
    int16_t outputDataIds[MAX_OUTPUTS];

    public:
    FGNode() {
        for (int i = 0; i < MAX_INPUTS; i++) {
            inputDataIds[i] = -1;
        }
        for (int i = 0; i < MAX_OUTPUTS; i++) {
            outputDataIds[i] = -1;
        }
    }

    G3DFrameGraph* getFrameGraph() {
        return m_frameGraph;
    }

    // Returns the next node in the graph
    virtual int execute(class G3DStackAllocator* nodeData) = 0;

    virtual void reset() {
        m_done = false;
    }

    FGNodeID getId() {
        return m_id;
    }

    void setIdentifier(FGNodeID id) {
        m_id = id;
    }


    int getInputDataId(int slot) {
        return inputDataIds[slot];
    }

    void setInputDataId(int slot, int id) {
        inputDataIds[slot] = id;
    }

    int getOutputDataId(int slot) {
        return outputDataIds[slot];
    }

    void setOutputDataId(int slot, int id) {
        outputDataIds[slot] = id;
    }

    G3DStackAllocator* getNodeDataAllocator();

    template <typename T, uint32_t typeHash>
    T* getInputData(int slot) {
        return getNodeDataAllocator()->get<T, typeHash>(inputDataIds[slot]);
    }

    template <typename T, uint32_t typeHash>
    T* allocateOutputData(int slot) {
        G3DStackAllocator* m_nodeDataAllocator = getNodeDataAllocator();
        T* data = m_nodeDataAllocator->allocate<T, typeHash>();
        outputDataIds[slot] = m_nodeDataAllocator->getId(data);
        return data;
    }

    template <typename T, uint32_t typeHash>
    T* getOutputData(int slot) {
        return getNodeDataAllocator()->get<T, typeHash>(outputDataIds[slot]);
    }


};