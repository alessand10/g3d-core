#pragma once
#include "scene-graph.h"
#include "allocators/pooled-allocator.h"
#include "components/component-pool.h"
#include "components/component-base.h"
#include <unordered_map>
#include <memory>
#include <vector>

#define MAX_ENTITIES 64
#define MAX_COMPONENT_POOLS 4

class G3DWorld {
    SceneGraph m_sceneGraph;
    PooledAllocator<G3DEntity> m_entityAllocator;
    void* m_componentPools[MAX_COMPONENT_POOLS];
    G3DEntity m_worldEntity;

    public:
    G3DWorld() {
        // Allocate space for 64 entities
        m_entityAllocator.init(MAX_ENTITIES * sizeof(G3DEntity), sizeof(G3DEntity));

        // Initialize the world entity as the root of the scene graph
        m_sceneGraph.init(&m_worldEntity);
    }


    int m_componentPoolCount = 0;
    template <typename T>
    ComponentPool<T>* getComponentPool() {
        static_assert(std::is_base_of<IG3DComponent, T>::value, "T must derive from IG3DComponent");
        ComponentTypeID typeId = T::typeId;
        if (typeId == -1){
            ComponentPool<T>* pool = new ComponentPool<T>();
            pool->init(MAX_ENTITIES);
            m_componentPools[m_componentPoolCount] = pool;

            // Assign the type id to the component pool
            T::typeId = m_componentPoolCount;
            m_componentPoolCount++;
            return pool;
        }
        else {
            return reinterpret_cast<ComponentPool<T>*>(m_componentPools[typeId]);
        }
    }

    G3DEntity* spawnEntity() {
        G3DEntity* entity = m_entityAllocator.allocate();

        // Set the id to identify components
        entity->entityId = m_entityAllocator.getChunkIndex(entity);

        entity->world = this;

        m_sceneGraph.addNode(entity, m_sceneGraph.getRoot());

        return entity;
    }

    void destroyEntity(G3DEntity* entity) {
        m_entityAllocator.deallocate(entity);
        entity->entityId = -1;
    }

    template <typename T>
    std::vector<T*> getComponents(EntityID entityId) {
        static_assert(std::is_base_of<IG3DComponent, T>::value, "T must derive from IG3DComponent");
        ComponentPool<T>* componentPool = getComponentPool<T>();
        return componentPool->getComponent(entityId);
    }

    template <typename T>
    T* addComponent(G3DEntity* entity) {
        static_assert(std::is_base_of<IG3DComponent, T>::value, "T must derive from IG3DComponent");
        ComponentPool<T>* componentPool = getComponentPool<T>();
        T* component = componentPool->allocateComponent();
        componentPool->addComponent(entity->entityId, component);
        return component;
    }

    void destroy() {
        m_entityAllocator.destroy();
    }
};