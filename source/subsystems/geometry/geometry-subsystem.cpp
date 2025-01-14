#include "geometry-subsystem.h"
#include "g3d-defines.h"
#include "g3d-resource.h"
#include "g3d-engine.h"

void G3DGeometrySubsystem::init(G3DEngine *engine)
{
    // Create the vertex and index buffers
    G3DBufferInfo vBufferInfo {
        .requestedSize = 11 * sizeof(float) * 5000, // Support 5000 vertices, each with 11 floats
        .isStaging = true,
        .usage = G3DBufferUsage::VERTEX_BUFFER
    };

    G3DBufferInfo iBufferInfo {
        .requestedSize = sizeof(uint32_t) * 3000, // Support 3000 indices
        .isStaging = true,
        .usage = G3DBufferUsage::INDEX_BUFFER
    };

    IG3DResourceFactory* resourceFactory = engine->getResourceFactory();

    m_vertexBuffer = resourceFactory->createBuffer(&vBufferInfo);
    m_indexBuffer = resourceFactory->createBuffer(&iBufferInfo);

    G3DBufferViewInfo vBufferViewInfo {
    };

    m_vertexBufferView = resourceFactory->createBufferView(m_vertexBuffer, &vBufferViewInfo);
    m_indexBufferView = resourceFactory->createBufferView(m_indexBuffer, &vBufferViewInfo);
}

uint32_t currentVertOffset = 0;
uint32_t currentIndOffset = 0;

MeshID G3DGeometrySubsystem::insertMesh(G3DMesh *mesh)
{
    G3DMeshInsertion* prevInsertion = m_insertedMeshes.size() > 0 ? &m_insertedMeshes[m_insertedMeshes.size() - 1] : nullptr;

    uint32_t vertexDataByteSize = mesh->getVertexCount() * mesh->getVertexSize();
    uint32_t indexDataByteSize = mesh->getIndexCount() * sizeof(uint32_t);

    m_vertexBuffer->loadFromHostMemory((char*)mesh->getSerialVertexData(), vertexDataByteSize, currentVertOffset);
    m_indexBuffer->loadFromHostMemory((char*)mesh->getIndexData(), indexDataByteSize, currentIndOffset);

    G3DMeshInsertion meshInsertion {
        .mesh = mesh,
        .vertexOffset = prevInsertion ? prevInsertion->vertexOffset + prevInsertion->mesh->getVertexCount() : 0,
        .indexOffset = prevInsertion ? prevInsertion->indexOffset + prevInsertion->mesh->getIndexCount() : 0
    };

    currentVertOffset += vertexDataByteSize;
    currentIndOffset += indexDataByteSize;

    m_insertedMeshes.push_back(meshInsertion);

    return m_insertedMeshes.size() - 1;
}

void G3DGeometrySubsystem::destroy()
{
    m_vertexBufferView->destroy();
    m_indexBufferView->destroy();
    m_vertexBuffer->destroy();
    m_indexBuffer->destroy();
}
