#pragma once
#include "graph.h"
#include "ecs-base.h"
#include <vector>

class SceneGraph {
    Graph<ECSBase*> m_sceneGraph;
    Graph<ECSBase*>::Node* m_root;

    public:

    void init(ECSBase* root) {
        m_root = m_sceneGraph.addNode(root);
    };

    Graph<ECSBase*>::Node* getRoot() {
        return m_root;
    };

    Graph<ECSBase*>::Node* addNode(ECSBase* node, Graph<ECSBase*>::Node* parent) {
        Graph<ECSBase*>::Node* newNode = m_sceneGraph.addNode(node);
        m_sceneGraph.addUndirectedEdge(parent, newNode);
        return newNode;
    };
    
};