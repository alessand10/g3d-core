#pragma once
#include "gclass.h"

class PooledAllocatorBase : public GClassProperties {
    public:
    virtual void destroy() = 0;
};