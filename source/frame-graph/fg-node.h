#pragma once
#include "gclass.h"
#include "allocators/allocator-v2.h"

#define MAX_INPUTS 5
#define MAX_OUTPUTS 5

// Represents the node type in the frame graph
typedef int FGNodeID;

class FGNode : public GClass {
    public:
    FGNodeID m_id;
    FGNodeID m_nextId = -1;

    Allocator* m_nodeDataAllocator = nullptr;

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

    virtual void execute(class Allocator* nodeData) = 0;


    FGNodeID getId() {
        return m_id;
    }

    void setIdentifier(FGNodeID id) {
        m_id = id;
    }

    void setNextNode(FGNodeID id) {
        m_nextId = id;
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

    template <typename T, uint32_t typeHash>
    T* getInputData(int slot) {
        return m_nodeDataAllocator->getByID<T, typeHash>(inputDataIds[slot]);

    }

    template <typename T, uint32_t typeHash>
    T* allocateOutputData(int slot) {
        T* data = m_nodeDataAllocator->allocate<T, typeHash>();
        outputDataIds[slot] = m_nodeDataAllocator->getUniqueID(data);
        return data;
    }

    template <typename T, uint32_t typeHash>
    T* getOutputData(int slot) {
        return m_nodeDataAllocator->getByID<T, typeHash>(outputDataIds[slot]);
    }


};