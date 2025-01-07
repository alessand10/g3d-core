#pragma once
#include "pooled-allocator.h"
#include "unordered_map"
#include "id-type.h"
#include "gclass.h"

/**
 * The pooled allocator manager 
 */
template <int N>
class PooledAllocatorManager {
    // PooledAllocatorBase* m_allocators[N];
    // int m_allocatorCount = 0;
    // std::unordered_map<GClassID, PooledAllocatorBase*> m_allocatorMap;

    public:
    template <typename T>
    PooledAllocator<T>* createAllocator(size_t numChunks) {
    //     static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
    //     size_t chunkSize = sizeof(T);
    //     PooledAllocator<T>* allocator = new PooledAllocator<T>();
    //     allocator->setClassID(m_allocatorCount);
    //     allocator->init(numChunks * chunkSize, chunkSize);
    //     m_allocators[m_allocatorCount++] = allocator;
    //     m_allocatorMap[T::m_id] = allocator;
    //     return allocator;
    };

    template <typename T>
    PooledAllocator<T>* getAllocator() {
    //     static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
    //     return static_cast<PooledAllocator<T>*>(m_allocatorMap[T::m_id]);
    };

    template <typename T>
    PooledAllocator<T>* getAllocator(GClassID id) {
    //     static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
    //     return static_cast<PooledAllocator<T>*>(m_allocators[id]);
    };

    template <typename T>
    T* allocate() {
    //     static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
    //     if (m_allocatorMap.find(T::m_id) == m_allocatorMap.end()) {
    //         return nullptr;
    //     }
    //     return getAllocator<T>()->allocate();
    };

    void destroy() {
    //     for (int i = 0; i < m_allocatorCount; i++) {
    //         m_allocators[i]->destroy();
    //     }
    };
    

};