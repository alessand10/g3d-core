#include "geometry-subsystem.h"
#include "g3d-defines.h"
#include "g3d-resource.h"
#include "g3d-engine.h"
#include "file-loader.h"
#include "mesh-helper.h"

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

MeshID G3DGeometrySubsystem::importOBJMesh(const char *path)
{
    GeometryData* meshData = nullptr;
    uint32_t numMeshes = 0;
    G3DFileLoader::Geometry::loadOBJ(path, &meshData, &numMeshes);

    G3DSerializeElements elementOrder[] = {
        G3DSerializeElements::POSITION,
        G3DSerializeElements::NORMAL,
        G3DSerializeElements::TEXCOORD,
        G3DSerializeElements::TANGENT
    };

    G3DSerializedMeshData serializedData;

    getSerializedMeshData(meshData, &serializedData, elementOrder, 4);

    m_meshes.push_back(G3DMesh());

    G3DMesh* mesh = &m_meshes[m_meshes.size() - 1];

    mesh->setSerialVertexData(serializedData.vertexData);
    mesh->setVertexCount(serializedData.vertexCount);
    mesh->setVertexSize(serializedData.vertexSize);
    mesh->setIndexData(serializedData.indexData);
    mesh->setIndexCount(serializedData.indexCount);
    mesh->setIndexSize(serializedData.indexSize);

    return insertMesh(mesh);
}

MeshID G3DGeometrySubsystem::insertMesh(G3DMesh *mesh)
{

    // G3DSerializeElements elementOrder[] = {
    //     G3DSerializeElements::POSITION,
    //     G3DSerializeElements::NORMAL,
    //     G3DSerializeElements::TEXCOORD,
    //     G3DSerializeElements::TANGENT
    // };

    // G3DSerializedMeshData serializedData;

    // getSerializedMeshData(meshData, &serializedData, elementOrder, 4);

    // mesh->setSerialVertexData(serializedData.vertexData);
    // mesh->setVertexCount(serializedData.vertexCount);
    // mesh->setVertexSize(serializedData.vertexSize);
    // mesh->setIndexData(serializedData.indexData);
    // mesh->setIndexCount(serializedData.indexCount);
    // mesh->setIndexSize(serializedData.indexSize);

    // meshOffsets[mesh] = std::make_pair(currentVertOffset / serializedData.vertexSize, currentIndOffset / serializedData.indexSize);


    G3DMeshInsertion* prevInsertion = m_insertedMeshes.size() > 0 ? &m_insertedMeshes[m_insertedMeshes.size() - 1] : nullptr;

    uint32_t vertexDataByteSize = mesh->getVertexCount() * mesh->getVertexSize();
    uint32_t indexDataByteSize = mesh->getIndexCount() * sizeof(uint32_t);

    m_vertexBuffer->loadFromHostMemory((char*)mesh->getSerialVertexData(), vertexDataByteSize, currentVertOffset);
    m_indexBuffer->loadFromHostMemory((char*)mesh->getIndexData(), indexDataByteSize, currentIndOffset);

    G3DMesh* previousMesh = prevInsertion ? &m_meshes[prevInsertion->id] : nullptr;
    

    uint32_t vertexOffset = prevInsertion ? (prevInsertion->vertexOffset + previousMesh->getVertexCount()) : 0;

    G3DMeshInsertion meshInsertion {
        .id = m_meshes.size() - 1,
        .vertexOffset = prevInsertion ? prevInsertion->vertexOffset + previousMesh->getVertexCount() : 0,
        .indexOffset = prevInsertion ? prevInsertion->indexOffset + previousMesh->getIndexCount() : 0
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
