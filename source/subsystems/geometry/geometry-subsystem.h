#pragma once
#include <vector>
#include "mesh.h"

struct G3DMeshInsertion {
    G3DMesh* mesh;
    uint32_t vertexOffset;
    uint32_t indexOffset;
};

class G3DGeometrySubsystem {
    std::vector<G3DMeshInsertion> m_insertedMeshes = {};
    
    class IG3DBuffer* m_vertexBuffer;
    class IG3DBufferView* m_vertexBufferView;

    class IG3DBuffer* m_indexBuffer;
    class IG3DBufferView* m_indexBufferView;

    public:
    G3DGeometrySubsystem() {};
    ~G3DGeometrySubsystem() {};

    void init(class G3DEngine* engine);
    MeshID insertMesh(G3DMesh* mesh);

    G3DMeshInsertion* getMeshInsertion(MeshID id) {
        return &m_insertedMeshes[id];
    }

    std::vector<G3DMeshInsertion>& getInsertedMeshes() {
        return m_insertedMeshes;
    }

    class IG3DBufferView* getVertexBufferView() { return m_vertexBufferView; }
    class IG3DBufferView* getIndexBufferView() { return m_indexBufferView; }

    void destroy();
};