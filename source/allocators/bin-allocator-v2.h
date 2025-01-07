#pragma once
#include "minimal-type-defines.h"
#include "gclass.h"
#include <stdexcept>
#include <unordered_map>

typedef unsigned char uint8_t;

struct BinAllocatorEntry {
    bool allocated = false;
    uint32_t size = 0U;
    uint32_t offset = 0U;
    uint32_t used = 0U;
};

enum class Bytes {
    TWO = 0b1 << 1U,
    FOUR = 0b1 << 2U,
    EIGHT = 0b1 << 3U,
    SIXTEEN = 0b1 << 4U,
    THIRTY_TWO = 0b1 << 5U,
};

template <typename K, typename V, int N>
struct BinAllocatorMap {
    uint8_t binAllocatorEntryIndices [N + 1]{};
    V* binAllocatorEntries = nullptr;
    uint8_t maxSupportedValues = 0;
    int numEntries = 0;
    void init(uint8_t maxSupportedValues) {
        // We use +1 because the final index is reserved for the invalid value
        binAllocatorEntries = new V[maxSupportedValues + 1];
        binAllocatorEntries[maxSupportedValues] = BinAllocatorEntry{false, 0U, 0U, 0U};
        std::fill_n(binAllocatorEntryIndices, N, maxSupportedValues);
        this->maxSupportedValues = maxSupportedValues;
    };

    void insert(K key, V value) {
        binAllocatorEntryIndices[key] = value;
    };

    V get(K key) {
        return binAllocatorEntryIndices[key];
    };

    bool has(K key) {
        return binAllocatorEntryIndices[key] != maxSupportedValues;
    };

    void remove(K key) {
        binAllocatorEntryIndices[key] = maxSupportedValues;
    };

    void clear() {
        std::fill_n(binAllocatorEntryIndices, N, maxSupportedValues);
    };

    void set(K key, V value) {
        if (binAllocatorEntryIndices[key] == maxSupportedValues) {
            binAllocatorEntryIndices[key] = numEntries;
            numEntries++;
        }
        binAllocatorEntries[binAllocatorEntryIndices[key]] = value;
    };

    V* getPtr(K key) {
        return &binAllocatorEntries[binAllocatorEntryIndices[key]];
    };

    inline V& operator[](K key) {
        return binAllocatorEntries[binAllocatorEntryIndices[key]];
    };

    void destroy() {
        delete[] binAllocatorEntries;
    };
};

class BinAllocator {
    void* memoryBlock;
    Bytes alignment;

    //std::unordered_map<GClassID, BinAllocatorEntry> m_binMap;
    BinAllocatorMap<GClassID, BinAllocatorEntry, 256> m_binMap;

    // Each index corresponds to a GClass ID, the value at the index is the index of the bin in the bin allocator entry array

    /**
     * Takes a pointer and aligns it to the next multiple of the alignment
     */
    void* alignPointer(void* ptr) {
        uintptr_t ptrInt = reinterpret_cast<uintptr_t>(ptr);
        uint32_t alignment = static_cast<uint32_t>(this->alignment);
        uint32_t remainder = ptrInt % alignment;
        if (remainder == 0) return ptr;
        ptrInt = ptrInt + (alignment - (ptrInt % alignment));
        return reinterpret_cast<void*>(ptrInt);
    }

    uint32_t alignOffset(uint32_t offset) {
        return (offset + static_cast<uint32_t>(alignment) - 1) & ~(static_cast<uint32_t>(alignment) - 1);
    }

    // The total amount of memory allocated
    uint32_t allocatedAmount = 0U;

    public:

    void init(uint32_t totalSize, uint8_t maxClasses, Bytes alignment) {
        this->alignment = alignment;
        memoryBlock = new char[totalSize, static_cast<uint32_t>(alignment)];

        m_binMap.init(maxClasses);
    };

    /**
     * Creates a bin in the allocator with the given size, mapping the bin to the given GClass id.
     */
    void createBin(GClassProperties gclass, uint32_t size) {
        if (gclass.getClassID() == -1) 
            throw std::runtime_error("GClass has not been registered in the engine");

        // Move allocatedAmount to the next aligned address
        allocatedAmount = alignOffset(allocatedAmount);

    
        BinAllocatorEntry entry;
        entry.size = size;
        entry.offset = allocatedAmount;
        entry.used = 0U;
        entry.allocated = true;
        m_binMap.set(gclass.getClassID(), entry);
        allocatedAmount += size;
    };

    bool hasBin(GClassProperties gclass) {
        return m_binMap.has(gclass.getClassID());
    };

    template <typename T>
    T* allocate()  {
        static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
        GClassProperties gclass = T::GetGClass();

        if (!hasBin(gclass)) 
            throw std::runtime_error("Bin does not exist for GClass");

        BinAllocatorEntry entry = m_binMap[gclass.getClassID()];

        if (entry.used + gclass.getSize() > entry.size) 
            throw std::runtime_error("Bin is full");

        char* newMemoryPtr = static_cast<char*>(memoryBlock) + entry.offset + entry.used;

        // align the pointer
        newMemoryPtr = static_cast<char*>(alignPointer(newMemoryPtr));

        T* ptr = new (newMemoryPtr) T();
        entry.used += gclass.getSize();
        // m_binMap[gclass.getClassID()] = entry;
        m_binMap.set(gclass.getClassID(), entry);
        return ptr;

    }

    template <typename T>
    inline T* safeCast(void* ptr) {
        static_assert(std::is_base_of<GClass, T>::value, "T must be a subclass of GClass");
        
        GClassProperties gclass = T::GetGClass();
        BinAllocatorEntry entry = m_binMap[T::GetGClass().getClassID()];

        if (!entry.allocated) 
            return nullptr;
            
        // Fail the cast if the pointer is not within the bin
        if (ptr < memoryBlock + entry.offset || ptr > memoryBlock + entry.offset + entry.size) 
            return nullptr;

        // Fail the cast if the pointer is not aligned to the size of the class
        if ((reinterpret_cast<uintptr_t>(ptr) - entry.offset) % gclass.getSize() != 0) 
            return nullptr;
        
        return reinterpret_cast<T*>(ptr);
    }


    void destroy() {
        m_binMap.destroy();
        delete[] memoryBlock;
    };
};