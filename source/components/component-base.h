#pragma once
#include "ecs-base.h"

typedef int ComponentTypeID;

typedef int ComponentID;

#define COMPONENT_BASE(type) \
    friend class G3DWorld;\
    public: \
    inline static ComponentTypeID typeId = -1; \
    private:

class IG3DComponent : ECSBase {

    public:
    virtual void init() = 0;
    virtual void update() = 0;
    virtual void destroy() = 0;
};