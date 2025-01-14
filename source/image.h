#pragma once
#include "inttypes.h"
#include <memory>

/**
 * Represents an image stored in host memory
 */
class Image {
    friend class ImageLoader;

    uint32_t m_width, m_height;
    uint32_t m_byteAlignment;
    uint32_t m_byteSize;

    std::unique_ptr<char[]> m_data;

    public: 
    uint32_t getByteSize() { return m_byteSize; }
    uint32_t getHeight() { return m_height; }
    uint32_t getWidth() { return m_width; }
    uint32_t getByteAlignment() { return m_byteAlignment; }
    char* getData() { return m_data.get(); }

};