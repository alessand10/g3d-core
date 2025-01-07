#pragma once
#include "pooled-allocator.h"

template <typename T>
class G3DIDType : public T {
    int m_id = -1;

    public:
    void setID(int id) {
        m_id = id;
    };
    int getID() {
        return m_id;
    };
};