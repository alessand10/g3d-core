#pragma once
#include "scene-graph.h"
#include "allocators/pooled-allocator.h"
#include "allocators/g3d-stack-allocator.h"
#include "components/component-pool.h"
#include "components/component-base.h"
#include <unordered_map>
#include <memory>
#include <vector>

#define MAX_ENTITIES 16
#define MAX_COMPONENT_POOLS 8

class G3DWorld {
    SceneGraph m_sceneGraph;
    PooledAllocator<G3DEntity> m_entityAllocator;
    G3DStackAllocator m_componentAllocator;
    G3DEntity m_worldEntity;

    // Stores the components for an entity, this will be replaced with a more efficient data structure
    std::unordered_map<EntityID, std::vector<int>> m_entityComponentMap;

    public:
    G3DWorld() {
        // Allocate space for 64 entities
        m_entityAllocator.init(MAX_ENTITIES * sizeof(G3DEntity), sizeof(G3DEntity));

        // Initialize the component allocator with 1024 bytes of space
        m_componentAllocator.init(1024);

        // Initialize the world entity as the root of the scene graph
        m_sceneGraph.init(&m_worldEntity);
    }

    EntityID spawnEntity() {
        G3DEntity* entity = m_entityAllocator.allocate();

        // Set the id to identify components
        entity->entityId = m_entityAllocator.getChunkIndex(entity);

        entity->world = this;

        m_sceneGraph.addNode(entity, m_sceneGraph.getRoot());

        return entity->entityId;
    }

    void destroyEntity(EntityID entityId) {
        G3DEntity* entity = m_entityAllocator[entityId];
        m_entityAllocator.deallocate(entity);
        entity->entityId = -1;
    }

    std::vector<ComponentID>& getComponents(EntityID entityId) {
        return m_entityComponentMap[entityId];
    }

    template <typename T, uint32_t typeHash>
    ComponentID addComponent(EntityID entityId) {
        static_assert(std::is_base_of<IG3DComponent, T>::value, "T must derive from IG3DComponent");
        ComponentID createdComponent = m_componentAllocator.allocate<T, typeHash>();
        m_entityComponentMap[entityId].push_back(createdComponent);

        return createdComponent;
    }

    bool entityHasComponents(EntityID entityId, uint32_t* componentHashes, int numComponents) {
        std::vector<ComponentID>& components = m_entityComponentMap[entityId];
        
        // Return false if more components are requested than the entity has
        if (components.size() < numComponents) {
            return false;
        }

        // There is space for more optimization in this loop
        for (int i = 0; i < numComponents; i++) {
            bool found = false;
            
            // For each component hash provided, check if the entity has a component with that hash
            for (ComponentID component : components) {
                if (m_componentAllocator.getGClassID(component) == componentHashes[i]) {

                    // If true, we can break from this loop
                    found = true;
                    break;
                }
            }

            if (!found) {
                return false;
            }
        }
        
        return true;
    }

    template <typename T, uint32_t typeHash>
    T* getComponent(ComponentID id) {
        return m_componentAllocator.get<T, typeHash>(id);
    }

    void destroy() {
        m_entityAllocator.destroy();
        m_componentAllocator.destroy();
    }
};