#pragma once
#include "graph.h"
#include "fg-node.h"
#include "fg-do-once-node.h"
#include "fg-for-each-node.h"
#include "fg-action-node.h"
#include "minimal-type-defines.h"
#include "allocators/pooled-allocator-manager.h"
#include "allocators/allocator-v2.h"
#include <stack>

#define MAX_NODES 24
#define MAX_NODE_TYPES 8

typedef int FGNodeDataID;

class G3DFrameGraph {
    Allocator m_nodeAllocator;
    Allocator m_nodeDataAllocator;

    FGNode* m_entryNode;
    FGNode* m_exitNode;
    public:


    FGDoOnceNode* addDoOnceNode() {
        FGDoOnceNode* entry = m_nodeAllocator.allocate<FGDoOnceNode>();
        entry->m_nodeDataAllocator = &m_nodeDataAllocator;
        int index = m_nodeAllocator.getUniqueID(entry);
        entry->setIdentifier(index);
        return entry;
    }

    FGForLoopNode* addForLoopNode(int iterationCount) {
        FGForLoopNode* entry = m_nodeAllocator.allocate<FGForLoopNode>();
        entry->m_nodeDataAllocator = &m_nodeDataAllocator;
        int index = m_nodeAllocator.getUniqueID(entry);
        int* iterationVariable = entry->allocateOutputData<CLASS_HASH(int)>(0); // Allocate the output data for the iterator variable
        int* numIterations = entry->allocateOutputData<CLASS_HASH(int)>(1); // Allocate the output data for the iteration count

        *iterationVariable = -1;
        *numIterations = iterationCount;
        
        entry->setIdentifier(index);
        return entry;
    }

    FGActionNode* addActionNode(void (*action)(FGActionNode* node)) {
        FGActionNode* entry = m_nodeAllocator.allocate<FGActionNode>();
        entry->m_nodeDataAllocator = &m_nodeDataAllocator;
        int index = m_nodeAllocator.getUniqueID(entry);
        entry->action = action;
        entry->setIdentifier(index);
        return entry;
    }

    void executeNode(FGNode* node) {
        node->execute(&m_nodeDataAllocator);
    }

    void init(class G3DEngine* engine);

    FGNode* getEntryNode() {
        return m_entryNode;
    }

    FGNode* getExitNode() {
        return m_exitNode;
    }

    template <typename T, uint32_t typeHash>
    T* addNodeData() {
        return m_nodeDataAllocator.allocate<T, typeHash>();
    }

    int getUniqueNodeDataID(void* ptr) {
        return m_nodeDataAllocator.getUniqueID(ptr);
    }

    template <typename T>
    T* getNodeData(FGNodeDataID id) {
        return m_nodeDataAllocator.getByID<T>(id);
    }


    void execute() {

        std::stack<FGNodeID> nodePath = {};
        nodePath.push(m_exitNode->getId());
        nodePath.push(m_entryNode->getId());

        FGNodeID currentNodeID = -1;
        while (!nodePath.empty()) {

            // Remove the top node from the stack
            currentNodeID = nodePath.top();
            nodePath.pop();

            FGDoOnceNode* doOnceNode = m_nodeAllocator.getByID<FGDoOnceNode>(currentNodeID);
            if (doOnceNode) {
                executeNode(doOnceNode);
                if (doOnceNode->m_nextId != -1)
                    nodePath.push(doOnceNode->m_nextId);
                continue;
            }

            FGForLoopNode* forEachNode = m_nodeAllocator.getByID<FGForLoopNode>(currentNodeID);
            if (forEachNode) {
                executeNode(forEachNode);
                if (forEachNode->getPendingReset()) {
                    forEachNode->reset();
                    nodePath.push(forEachNode->m_nextId);
                }
                else if (forEachNode->getCurrentIteration() < forEachNode->getIterationCount()) {
                    nodePath.push(currentNodeID);
                    nodePath.push(forEachNode->repeatNodeID);
                }
                continue;
            }

            FGActionNode* actionNode = m_nodeAllocator.getByID<FGActionNode>(currentNodeID);
            if (actionNode) {
                executeNode(actionNode);
                if (actionNode->m_nextId != -1) 
                    nodePath.push(actionNode->m_nextId);
                continue;
            }

        }
    }
};