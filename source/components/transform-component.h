#pragma once
#include "component-base.h"
#include "glm/glm.hpp"

class G3DTransformComponent : public IG3DComponent {
    COMPONENT_BASE(G3DWorldTransformComponent);

    public:
    glm::mat4 m_worldMatrix;
    glm::mat4 m_scaleMatrix;
    glm::mat4 m_rotationMatrix;
    glm::mat4 m_translationMatrix;
    bool m_isDirty = true;

    bool m_translationLocked = false;
    bool m_rotationLocked = false;
    bool m_scaleLocked = false;

    void init() override {};
    void update() override {};
    void destroy() override {};

    G3DTransformComponent();
    ~G3DTransformComponent();
    glm::mat4 getTransformMatrix();
    void translate(glm::vec3 translation);
    void rotate(float angle, glm::vec3 axis);
    void scale(glm::vec3 scale);

    glm::vec3 getPosition();
    glm::vec3 getForwardDirection();
    glm::vec3 getBackwardDirection();
    glm::vec3 getUpDirection();
    glm::vec3 getRightDirection();
    glm::vec3 getLeftDirection();
};