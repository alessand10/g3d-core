#pragma once
#include "component-base.h"

typedef int MeshID;

class G3DRenderableComponent : public IG3DComponent {
    COMPONENT_BASE(G3DRenderableComponent);

    MeshID m_meshId;

    public:
    void init() override {};
    void update() override {};
    void destroy() override {};

};