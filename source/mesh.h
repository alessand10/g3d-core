#pragma once

typedef unsigned int uint32_t;

typedef int MeshID;

class G3DMesh {
    protected:
    const char* m_name;

    float* m_serialVertexData;
    unsigned int m_vertexCount;
    unsigned int m_vertexSize;

    uint32_t* m_indexData;
    unsigned int m_indexCount;
    unsigned int m_indexSize;

    public:
    void setName(const char* name) {
        m_name = name;
    }

    const char* getName() {
        return m_name;
    }

    void setSerialVertexData(float* vertexData) {
        m_serialVertexData = vertexData;
    }

    float* getSerialVertexData() {
        return m_serialVertexData;
    }

    void setVertexCount(unsigned int vertexCount) {
        m_vertexCount = vertexCount;
    }

    unsigned int getVertexCount() {
        return m_vertexCount;
    }

    void setVertexSize(unsigned int vertexSize) {
        m_vertexSize = vertexSize;
    }

    unsigned int getVertexSize() {
        return m_vertexSize;
    }

    void setIndexData(uint32_t* indexData) {
        m_indexData = indexData;
    }

    uint32_t* getIndexData() {
        return m_indexData;
    }

    void setIndexCount(unsigned int indexCount) {
        m_indexCount = indexCount;
    }

    unsigned int getIndexCount() {
        return m_indexCount;
    }

    void setIndexSize(unsigned int indexSize) {
        m_indexSize = indexSize;
    }

    unsigned int getIndexSize() {
        return m_indexSize;
    }
};