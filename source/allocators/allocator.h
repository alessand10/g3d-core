#pragma once
#include <algorithm>
#include <tuple>
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

// template <typename T, typename... Ts>
// class I {
//     template <typename T, typename... Is>
//     class IHelper{
//         std::tuple<Is...> indices;

//         constexpr size_t getTypeIndex() {
//             std::tuple<Is...> indices;
//             return std::get<T>(indices);
//         } 
//     }

//     constexpr getTypeIndex() {
//         IHelper<T, std::index_sequence_for<Ts...>> helper;
//         return helper.getTypeIndex();
//     }
    
//};

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




template <typename ... Ts>
class Allocator {
    char* memoryBlock;
    uint32_t size;
    uint32_t usedBytes;

    uint32_t preComputedBitMask = 0U;

    bool validCasts[sizeof...(Ts)][256]{};

    // All power of 2 sizes
    uint32_t elementPowerSize;

    uint32_t* freeStack;

    template <typename T>
    inline bool getValidCast(size_t index) {
        return validCasts[getTypeIndex<0, T, Ts...>()][index];
    }

    template <typename T>
    inline void setValidCast(size_t index) {
        validCasts[getTypeIndex<0, T, Ts...>()][index] = true;
    }

    template <typename T>
    inline void clearValidCast(size_t index) {
        validCasts[getTypeIndex<0, T, Ts...>()][index] = false;
    }

    void populateFreeStack(uint32_t numElements) {
        for (int i = numElements - 1, index = 0; i >= 0; i--, index++) {
            freeStack[i] = index;
        }
    }

    std::tuple<Ts** ...> preComputedCasts;
    void populatePreComputedCasts(uint32_t numElements) {
        preComputedCasts = std::make_tuple(new Ts*[numElements]{}...);
    }


    void populateValidCasts(uint32_t numElements, uint32_t numTypes) {
        //validCasts = new bool[numTypes][numElements]{};
    }


    public:

    char* getNextAlignedPointer(const char* ptr) {
        return (char*)(((uintptr_t)ptr + size - 1) & ~(size - 1));
    }

    uint32_t getNextAlignedOffset(const uint32_t offset) {
        return (offset + size - 1) & ~(size - 1);
    }

    uint32_t getNearestPowerOf2(uint32_t num) {
        uint32_t power = 1U;
        while ((1U << power) < num) {
            power++;
        }
        return power + 1U;
    }

    void init(uint32_t numElements) {
        // Get the largest size needed to store any of the types
        constexpr uint32_t largestSize = std::max({sizeof(Ts)...});

        elementPowerSize = getNearestPowerOf2(largestSize);

        // compute the total size
        uint32_t totalSize = (1U << elementPowerSize) * numElements;

        memoryBlock = new (std::align_val_t(1U << elementPowerSize)) char[totalSize];
        size = totalSize;
        usedBytes = 0U;
        freeStack = new uint32_t[numElements];
        populateFreeStack(numElements);
        populatePreComputedCasts(numElements);
        populateValidCasts(numElements, sizeof...(Ts));
        preComputedBitMask = (1U << elementPowerSize) - 1U;
    };

    template <typename T>
    T* allocate() {
        
        if (usedBytes + (1 << elementPowerSize) > size) {
            throw std::runtime_error("Allocator is full");
        }

        uint32_t freeBytes = size - usedBytes;
        uint32_t index = freeStack[(freeBytes >> elementPowerSize) - 1U];
        uint32_t offset = index << elementPowerSize;
        T* ptr = new (memoryBlock + offset) T();
        usedBytes += (1U << elementPowerSize);

        setValidCast<T>(index);

        return ptr;
    };

    template <typename T>
    void deallocate(T* ptr) {
        uint32_t offset = (uint32_t)((char*)ptr - memoryBlock);
        uint32_t index = offset >> elementPowerSize;
        uint32_t freeBytes = size - usedBytes;
        usedBytes -= (1U << elementPowerSize);
        freeStack[freeBytes >> elementPowerSize] = index;

        clearValidCast<T>(index);
    };


    template <typename T>
    inline T* safeCast(void* ptr) {

        if ((uintptr_t)ptr & preComputedBitMask) {
            return nullptr; // Cast fails, the element is not aligned
        }

        int offset = reinterpret_cast<char*>(ptr) - memoryBlock;
        uint32_t index = offset >> elementPowerSize;
        if (getValidCast<T>(index)) {
            return reinterpret_cast<T*>(ptr);
        }
        return nullptr;
    };

    void destroy() {
        delete[] freeStack;
        delete[] memoryBlock;
    };
};