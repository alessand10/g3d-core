#pragma once
#include <algorithm>
#include <tuple>
#include <stdexcept>
#include "gclass.h"
#include <unordered_map>

/**
 * Allo
 */

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef size_t uintptr_t;
 

class G3DStackAllocator {

    struct ElementHeader {
        uint32_t supportedType;
        uint32_t uniqueID;
    };
    static constexpr size_t headerSize = sizeof(ElementHeader);


    char* memoryBlock;
    uint32_t size;
    uint32_t stackPointer;
    uint32_t insertIndex = 0U;

    uint32_t m_idToTypeMap[128] = {};
    char* m_idToPtrMap[128] = {};

    inline char* getWrapperPointer(void* ptr) {
        return (char*)ptr - headerSize;
    }

    public:

    void init(uint32_t size) {
        memoryBlock = new char[size];
        this->size = size;
        stackPointer = 0U;

        std::fill_n(m_idToTypeMap, 128, 0);
        std::fill_n(m_idToPtrMap, 128, nullptr);
    };

    template <typename T, uint32_t typeHash>
    int allocate() {

        m_idToTypeMap[insertIndex] = typeHash;
        m_idToPtrMap[insertIndex] = memoryBlock + stackPointer + headerSize;
        
        if (stackPointer + (sizeof(T) + headerSize) > size) {
            throw std::runtime_error("Allocator is full");
        }

        char* offset = memoryBlock + stackPointer;
        ElementHeader* ptr = new (memoryBlock + stackPointer) ElementHeader{typeHash, insertIndex};
        T* element = new (offset + sizeof(ElementHeader)) T();

        stackPointer += sizeof(T) + headerSize;

        // Returns the wrapped contents, not the element wrapper
        return insertIndex++;
    };

    template <typename T>
    int allocate() {

        m_idToTypeMap[insertIndex] = T::getGClassID();
        m_idToPtrMap[insertIndex] = memoryBlock + stackPointer + headerSize;

        static_assert(std::is_base_of<GClass, T>::value, "T must be a base of GClass");

        if (stackPointer + (sizeof(T) + headerSize) > size) {
            throw std::runtime_error("Allocator is full");
        }

        char* offset = memoryBlock + stackPointer;
        ElementHeader* ptr = new (memoryBlock + stackPointer) ElementHeader{T::getGClassID(), insertIndex};
        T* element = new (offset + sizeof(ElementHeader)) T();
        
        stackPointer += sizeof(T) + headerSize;

        // Returns the wrapped contents, not the element wrapper
        return insertIndex++;
    };

    int getId(void* ptr) {
        char* wrapperPtr = getWrapperPointer(ptr);
        return reinterpret_cast<ElementHeader*>(wrapperPtr)->uniqueID;
    }

    template <typename T>
    void deallocate(T* ptr) {
        // char* wrapperPtr = getWrapperPointer(ptr);
        // uint32_t offset = (uint32_t)((char*)wrapperPtr - memoryBlock);
        // uint32_t index = offset >> elementPowerSize;
        // uint32_t freeBytes = size - usedBytes;
        // usedBytes -= (1U << elementPowerSize);
        // freeStack[freeBytes >> elementPowerSize] = index;
    };


    template <typename T>
    inline T* get(int id) {
        // Make sure T is a base of GClass
        static_assert(std::is_base_of<GClass, T>::value, "T must be a base of GClass");

        char* wrapperPtr = getWrapperPointer(m_idToPtrMap[id]);

        // Check if we have inserted the pointer into the map
        if (m_idToTypeMap[id] == T::getGClassID()) {
            return reinterpret_cast<T*>(m_idToPtrMap[id]);
        }
        
        return nullptr;
    };

    template <typename T, uint32_t typeHash>
    inline T* get(int id) {

        char* wrapperPtr = getWrapperPointer(m_idToPtrMap[id]);

        // Check if we have inserted the pointer into the map
        if (m_idToTypeMap[id] == typeHash) {
            return reinterpret_cast<T*>(m_idToPtrMap[id]);
        }
        
        return nullptr;
    };


    inline uint32_t getGClassID(int id) {
        return reinterpret_cast<ElementHeader*>(getWrapperPointer(m_idToPtrMap[id]))->supportedType;
    }

    void destroy() {
        delete[] memoryBlock;
    };
};