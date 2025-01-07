#pragma once
#include "ecs-base.h"

typedef int EntityID;

/**
 * Represents an entity in the game world.
 */
class G3DEntity : public ECSBase {
    friend class G3DWorld;
    protected:
    EntityID entityId;
    class G3DWorld* world;

    public:
    EntityID getEntityId() {
        return entityId;
    };

    G3DWorld* getWorld() {
        return world;
    };

    virtual void init() override {};
    virtual void update() override {};
    virtual void destroy() override {};
};