#pragma once
#include <vector>
#include "mesh.h"

struct G3DMeshInsertion {
    MeshID id;
    uint32_t vertexOffset;
    uint32_t indexOffset;
};

class G3DGeometrySubsystem {
    std::vector<G3DMesh> m_meshes = {};
    std::vector<G3DMeshInsertion> m_insertedMeshes = {};
    
    class IG3DBuffer* m_vertexBuffer;
    class IG3DBufferView* m_vertexBufferView;

    class IG3DBuffer* m_indexBuffer;
    class IG3DBufferView* m_indexBufferView;

    public:
    G3DGeometrySubsystem() {};
    ~G3DGeometrySubsystem() {};

    void init(class G3DEngine* engine);

    MeshID importOBJMesh(const char* path);
    MeshID insertMesh(G3DMesh* mesh);

    G3DMeshInsertion* getMeshInsertion(MeshID id) {
        return &m_insertedMeshes[id];
    }

    std::vector<G3DMeshInsertion>& getInsertedMeshes() {
        return m_insertedMeshes;
    }

    class IG3DBuffer* getVertexBuffer() { return m_vertexBuffer; }
    class IG3DBuffer* getIndexBuffer() { return m_indexBuffer; }
    class IG3DBufferView* getVertexBufferView() { return m_vertexBufferView; }
    class IG3DBufferView* getIndexBufferView() { return m_indexBufferView; }

    G3DMesh* getMesh(MeshID id) {
        return &m_meshes[id];
    }

    void destroy();
};