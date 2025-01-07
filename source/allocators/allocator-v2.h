#pragma once
#include <algorithm>
#include <tuple>
#include <stdexcept>
#include "gclass.h"

/**
 * Allo
 */


enum class AllocSize {
    K2 = 2U,
    K4 = 4U,
    K8 = 8U,
    K16 = 16U,
    K32 = 32U,
};
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef size_t uintptr_t;


template <int I = 0, typename C, typename T, typename... Is>
constexpr size_t getTypeIndex() {
    if constexpr (std::is_same_v<C, T>) {
        return I;
    } else {
        return getTypeIndex<I + 1, C, Is...>();
    }
}

template <int I, typename C>
constexpr size_t getTypeIndex() {
    return -1; // Return an invalid index if the type is not found
}
 

class Allocator {

    struct ElementHeader {
        uint32_t supportedType;
    };
    static constexpr size_t headerSize = sizeof(ElementHeader);

    uint32_t maxElementSize = 64U;


    char* memoryBlock;
    uint32_t size;
    uint32_t usedBytes;

    uint32_t preComputedBitMask = 0U;
    uint32_t preComputedShift = 0U;

    // All power of 2 sizes
    uint32_t elementPowerSize;

    uint32_t* freeStack;

    void populateFreeStack(uint32_t numElements) {
        for (int i = numElements - 1, index = 0; i >= 0; i--, index++) {
            freeStack[i] = index;
        }
    }

    uint32_t getNearestPowerOf2(uint32_t num) {
        uint32_t power = 1U;
        while ((1U << power) < num) {
            power++;
        }
        return power + 1U;
    }   

    char* getNextAlignedPointer(const char* ptr) {
        return (char*)(((uintptr_t)ptr + size - 1) & ~(size - 1));
    }

    uint32_t getNextAlignedOffset(const uint32_t offset) {
        return (offset + size - 1) & ~(size - 1);
    }
    
    inline char* getWrapperPointer(void* ptr) {
        return (char*)ptr - headerSize;
    }

    inline bool isPointerInMemoryBounds(void* ptr) {
        if ((uintptr_t)ptr & preComputedBitMask &&
            ptr < memoryBlock || ptr > memoryBlock + size) {
            return false;
        }
        return true;
    }

    public:

    void init(uint32_t numElements, uint32_t maxElementSize) {
        // Get the largest size needed to store any of the types
        uint32_t largestSize = maxElementSize + headerSize;
        this->maxElementSize = maxElementSize;

        elementPowerSize = getNearestPowerOf2(largestSize);

        // compute the total size
        uint32_t totalSize = (1U << elementPowerSize) * numElements;

        memoryBlock = new (std::align_val_t(1U << elementPowerSize)) char[totalSize];
        size = totalSize;
        usedBytes = 0U;
        freeStack = new uint32_t[numElements];
        populateFreeStack(numElements);
        preComputedShift = 1U << elementPowerSize;
        preComputedBitMask = preComputedShift - 1U;
    };

    template <typename T, uint32_t typeHash>
    T* allocate() {
        if (sizeof(T) > maxElementSize) {
            throw std::runtime_error("T is too large for this allocator");
        }
        
        if (usedBytes + (1 << elementPowerSize) > size) {
            throw std::runtime_error("Allocator is full");
        }

        uint32_t freeBytes = size - usedBytes;
        uint32_t index = freeStack[(freeBytes >> elementPowerSize) - 1U];
        uint32_t offset = index << elementPowerSize;
        ElementHeader* ptr = new (memoryBlock + offset) ElementHeader{typeHash};
        T* element = new (ptr + 1) T();
        usedBytes += (1U << elementPowerSize);

        // Returns the wrapped contents, not the element wrapper
        return reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + headerSize);
    };

    template <typename T>
    T* allocate() {

        static_assert(std::is_base_of<GClass, T>::value, "T must be a base of GClass");
        if (sizeof(T) > maxElementSize) {
            throw std::runtime_error("T is too large for this allocator");
        }
        
        if (usedBytes + (1 << elementPowerSize) > size) {
            throw std::runtime_error("Allocator is full");
        }

        uint32_t freeBytes = size - usedBytes;
        uint32_t index = freeStack[(freeBytes >> elementPowerSize) - 1U];
        uint32_t offset = index << elementPowerSize;
        ElementHeader* ptr = new (memoryBlock + offset) ElementHeader{T::getGClassID()};
        T* element = new (ptr + 1) T();
        usedBytes += (1U << elementPowerSize);

        // Returns the wrapped contents, not the element wrapper
        return reinterpret_cast<T*>(reinterpret_cast<char*>(ptr) + headerSize);
    };

    template <typename T>
    void deallocate(T* ptr) {
        char* wrapperPtr = getWrapperPointer(ptr);
        uint32_t offset = (uint32_t)((char*)wrapperPtr - memoryBlock);
        uint32_t index = offset >> elementPowerSize;
        uint32_t freeBytes = size - usedBytes;
        usedBytes -= (1U << elementPowerSize);
        freeStack[freeBytes >> elementPowerSize] = index;
    };


    template <typename T>
    inline T* safeCast(void* ptr) {
        // Make sure T is a base of GClass
        static_assert(std::is_base_of<GClass, T>::value, "T must be a base of GClass");

        char* wrapperPtr = getWrapperPointer(ptr);

        if ((uintptr_t)wrapperPtr & preComputedBitMask) {
            return nullptr; // Cast fails, the element is not aligned
        }

        if (ptr < memoryBlock || ptr > memoryBlock + size) {
            return nullptr; // Cast fails, the pointer is outside of bounds
        }

        constexpr size_t typeID = T::getGClassID();
        if (reinterpret_cast<ElementHeader*>(wrapperPtr)->supportedType == typeID) 
            return reinterpret_cast<T*>(ptr);
        return nullptr;
    };

    template <typename T, uint32_t typeHash>
    inline T* safeCast(void* ptr) {

        char* wrapperPtr = getWrapperPointer(ptr);

        if ((uintptr_t)wrapperPtr & preComputedBitMask) {
            return nullptr; // Cast fails, the element is not aligned
        }

        if (ptr < memoryBlock || ptr > memoryBlock + size) {
            return nullptr; // Cast fails, the pointer is outside of bounds
        }

        if (reinterpret_cast<ElementHeader*>(wrapperPtr)->supportedType == typeHash) 
            return reinterpret_cast<T*>(ptr);
        return nullptr;
    };

    /**
     * This ID uniquely identifies elements in the pool, it is the index of the element in the pool
     */
    int getUniqueID(void* ptr) {
        if (!isPointerInMemoryBounds(getWrapperPointer(ptr))) {
            return -1;
        }
        else 
            return ((char*)ptr - memoryBlock) >> elementPowerSize;
    }

    template <typename T>
    T* getByID(int uniqueID) {
        return safeCast<T>(memoryBlock + (uniqueID << elementPowerSize) + headerSize);
    }

    template <typename T, uint32_t typeHash>
    T* getByID(int uniqueID) {
        return safeCast<T, typeHash>(memoryBlock + (uniqueID << elementPowerSize) + headerSize);
    }


    inline int getGClassID(void* ptr) {
        if (!isPointerInMemoryBounds(getWrapperPointer(ptr))) {
            return -1;
        }
        else 
            return reinterpret_cast<GClass*>(ptr)->getObjectGClassID();
    }

    void destroy() {
        delete[] freeStack;
        delete[] memoryBlock;
    };
};