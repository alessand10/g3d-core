#pragma once

#include <cstdint>
#include <cstddef>
typedef uint32_t GClassID;


// FNV-1a compile-time hash
constexpr std::uint32_t fnv1a(const char* str, std::size_t i = 0, std::uint32_t hash = 2166136261u) {
    return str[i] == '\0' 
        ? hash 
        : fnv1a(str, i + 1, (hash ^ static_cast<unsigned char>(str[i])) * 16777619u);
}

#define STRING_HASH(s) fnv1a(#s)

#define CAST_TO(type) <type, STRING_HASH(#type)>

#define CLASS_HASH(T) T, STRING_HASH(T)


class GClassProperties {
    friend class G3DEngine;
    GClassID m_id = 0U;
    void setClassID(GClassID id) { m_id = id; }
    size_t m_size = 0;
    void setSize(size_t size) { m_size = size; }

    public:
    inline GClassID getClassID() { return m_id; } \
    inline size_t getSize() { return m_size; }
};

class GClass {
    public:
    virtual GClassID getObjectGClassID() = 0;
};

#define GCLASS_BODY(T) \
    protected: \
    inline static GClassProperties m_GClassProperties; \
    public: \
    virtual GClassID getObjectGClassID() override { return STRING_HASH(T); } \
    constexpr static GClassID getGClassID() { return STRING_HASH(T); } \
    static GClassProperties* GetGClass() { return &m_GClassProperties; } 
    
    