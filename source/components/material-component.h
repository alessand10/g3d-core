#pragma once
#include "component-base.h"


class G3DMaterialComponent : public IG3DComponent {
    COMPONENT_BASE(G3DMaterialComponent);

    int materialId = -1;

    public:
    void init() override {};
    void update() override {};
    void destroy() override {};

    void setMaterialId(int id) {
        materialId = id;
    }

    int getMaterialId() {
        return materialId;
    }
};