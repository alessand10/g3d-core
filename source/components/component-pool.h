#pragma once
#include "allocators/pooled-allocator.h"
#include <map>
#include "entity.h"


template <typename T>
class ComponentPool {
    private:

    // A pooled allocator for cameras
    PooledAllocator<T> m_componentPool;
    std::map<T*, EntityID> m_componentEntityMap;

    public:
    ComponentPool() {
        T::typeId = -1;
    };

    void init(int supportedComponentCount) {
        m_componentPool.init(supportedComponentCount * sizeof(T), sizeof(T));
    };

    void addComponent(EntityID entityId, T* component) {
        m_componentEntityMap[component] = entityId;
    };

    EntityID* getEntity(T* component) {
        return &m_componentEntityMap[component];
    };

    T* allocateComponent() {
        return m_componentPool.allocate();
    };

    void deallocateComponent(T* component) {
        m_componentPool.deallocate(component);
    };

    int getComponentCount() {
        return m_componentPool.getNumUsedChunks();
    };

    void destroy() {
        m_componentPool.destroy();
    };
};