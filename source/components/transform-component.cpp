#include "transform-component.h"
#include "glm/ext/matrix_transform.hpp"

G3DTransformComponent::G3DTransformComponent()
{
    m_worldMatrix = glm::identity<glm::mat4>();
    m_scaleMatrix = glm::identity<glm::mat4>();
    m_rotationMatrix = glm::identity<glm::mat4>();
    m_translationMatrix = glm::identity<glm::mat4>();
}

G3DTransformComponent::~G3DTransformComponent()
{
}

glm::mat4 G3DTransformComponent::getTransformMatrix()
{
    if (m_isDirty)
    {
        m_worldMatrix = m_translationMatrix * m_rotationMatrix * m_scaleMatrix;
        m_isDirty = false;
    }
    return m_worldMatrix;
}

void G3DTransformComponent::translate(glm::vec3 translation)
{
    m_isDirty = true;
    m_translationMatrix = glm::translate(m_translationMatrix, translation);
}

void G3DTransformComponent::rotate(float angle, glm::vec3 axis)
{
    m_isDirty = true;
    m_rotationMatrix = glm::rotate(m_rotationMatrix, angle, axis);
}

void G3DTransformComponent::scale(glm::vec3 scale)
{
    m_isDirty = true;
    m_scaleMatrix = glm::scale(m_scaleMatrix, scale);
}

glm::vec3 G3DTransformComponent::getPosition()
{
    glm::mat4 worldMatrix = getTransformMatrix();
    return glm::vec3{worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]};
}


glm::vec3 G3DTransformComponent::getForwardDirection()
{
    glm::mat4 worldMatrix = getTransformMatrix();
    return glm::vec3{worldMatrix[0][2], worldMatrix[1][2], worldMatrix[2][2]};
}

glm::vec3 G3DTransformComponent::getBackwardDirection()
{
    return -1.f * getForwardDirection();
}

glm::vec3 G3DTransformComponent::getUpDirection()
{
    glm::mat4 worldMatrix = getTransformMatrix();
    return glm::vec3{worldMatrix[0][1], worldMatrix[1][1], worldMatrix[2][1]};
}

glm::vec3 G3DTransformComponent::getLeftDirection()
{
    glm::mat4 worldMatrix = getTransformMatrix();
    return glm::vec3{worldMatrix[0][0], worldMatrix[1][0], worldMatrix[2][0]};
}

glm::vec3 G3DTransformComponent::getRightDirection()
{
    return -1.f * getLeftDirection();
}
