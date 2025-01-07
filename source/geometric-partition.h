#pragma once

/**
 * Represents a partition of geometry within a 3D scene.
 * This structure is used to organize geometry.
 * 
 * Only one geometric partition can be used for rendering at a time.
 */
class G3DGeometricPartition {
    class IG3DBuffer* m_vertexBuffer;
    class IG3DBuffer* m_indexBuffer;

    void init();
};