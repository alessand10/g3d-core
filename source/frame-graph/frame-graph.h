#pragma once
#include "graph.h"
#include "fg-node.h"
#include "fg-do-once-node.h"
#include "fg-for-loop-node.h"
#include "fg-action-node.h"
#include "fg-binary-sequence-node.h"
#include "minimal-type-defines.h"
#include "allocators/pooled-allocator-manager.h"
#include "allocators/g3d-stack-allocator.h"
#include <stack>

#define MAX_NODES 24
#define MAX_NODE_TYPES 8
#define STACK_SIZE 64

typedef int FGID;

class G3DFrameGraph {
    G3DStackAllocator m_nodeFGData;


    class IG3DCommandListAllocator* m_commandListAllocator;

    // Initially, the whole graph will get one command list, but this will change in the future when subgraphs are added
    class IG3DCommandList* m_commandList;

    FGNode* m_nodes[MAX_NODES];

    FGNodeID m_entryNodeId;
    public:

    IG3DCommandList* getCommandList() {
        return m_commandList;
    }

    G3DStackAllocator* getNodeDataAllocator() {
        return &m_nodeFGData;
    }

    template <typename T>
    FGID addNode() {
        FGNodeID nodeId = m_nodeFGData.allocate<T>();
        T* entry = m_nodeFGData.get<T>(nodeId);
        entry->m_frameGraph = this;
        int index = m_nodeFGData.getId(entry);
        entry->setIdentifier(index);
        entry->m_frameGraph = this;
        m_nodes[index] = entry;
        return nodeId;
    }

    void executeNode(FGNode* node) {
        node->execute(&m_nodeFGData);
    }

    void init(class G3DEngine* engine);

    void setEntryNode(FGNodeID nodeId) {
        m_entryNodeId = nodeId;
    }

    template <typename T>
    FGID addNodeData() {
        static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
        return m_nodeFGData.allocate<T>();
    }

    template <typename T, uint32_t typeHash>
    FGID addNodeData() {
        return m_nodeFGData.allocate<T, typeHash>();
    }

    int getId(void* ptr) {
        return m_nodeFGData.getId(ptr);
    }

    template <typename T>
    T* get(FGID id) {
        return m_nodeFGData.get<T>(id);
    }

    template <typename T, uint32_t typeHash>
    T* get(FGID id) {
        return m_nodeFGData.get<T, typeHash>(id);
    }

    uint16_t stackPointer = 0U;
    FGNodeID nodeStack[STACK_SIZE];

    inline void push(FGNodeID id) {
        m_nodes[id]->reset();
        nodeStack[stackPointer++] = id;
    }

    inline FGNodeID pop() {
        return nodeStack[--stackPointer];
    }

    inline bool empty() {
        return stackPointer == 0;
    }

    inline FGNodeID top() {
        return nodeStack[stackPointer - 1];
    }

    void execute() {

        push(m_entryNodeId);

        FGNodeID currentNodeID = -1;
        while (!empty()) {

            // Get the top node from the stack
            currentNodeID = top();
            FGNode* currentNode = m_nodes[currentNodeID];

            // Execute the node
            int nextNodeID = currentNode->execute(&m_nodeFGData);

            if (nextNodeID != -1) 
                push(nextNodeID);
            else 
                pop();

        }
    }
};