#pragma once
#include <list>
#include "pooled-allocator-base.h"
#include <iostream>

/**
 * A memory allocator that allocates an initial block of memory and then
 * allocates memory from that block in fixed-size chunks.
 */

template <typename T>
class PooledAllocator : public PooledAllocatorBase {
    private:
    std::list<T*> m_freeList;
    T* m_memoryBlock;

    // The size of the full memory block allocated
    size_t m_blockSize;

    // The size of each individual chunk
    size_t m_chunkSize;

    public:
    void init(size_t totalSize, size_t chunkSize) {
        m_blockSize = totalSize;
        m_chunkSize = chunkSize;

        m_memoryBlock = new T[m_blockSize];
        for (size_t i = 0; i < m_blockSize; i += m_chunkSize) {
            m_freeList.push_back(&m_memoryBlock[i]);
        }
    };

    // Returns an integer representing the chunk number of the given pointer
    size_t getChunkIndex(T* ptr) {
        return (ptr - m_memoryBlock) / m_chunkSize;
    };


    size_t getTotalChunks() {
        return m_blockSize / m_chunkSize;
    };

    size_t getNumFreeChunks() {
        return m_freeList.size();
    };

    size_t getNumUsedChunks() {
        return getTotalChunks() - getNumFreeChunks();
    };

    T* operator[](size_t index) {
        return &m_memoryBlock[index * m_chunkSize];
    };

    T* allocate() {
        if (m_freeList.empty()) {
            return nullptr;
        }

        T* result = m_freeList.front();
        m_freeList.pop_front();
        return result;
    };

    void deallocate(T* ptr) {
        m_freeList.push_back(ptr);
    };

    virtual void destroy() {
        delete[] m_memoryBlock;
    };
};