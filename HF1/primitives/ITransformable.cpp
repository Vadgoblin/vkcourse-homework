#include "ITransformable.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


void ITransformable::setScale(const float x, const float y, const float z)
{
    m_scale =  glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void ITransformable::setPosition(const float x, const float y, const float z)
{
    m_position = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void ITransformable::setRotation(float rx, float ry, float rz)
{
    rx                    = glm::radians(rx);
    ry                    = glm::radians(ry);
    rz                    = glm::radians(rz);
    const glm::vec3 angle = glm::vec3(rx, ry, rz);
    const glm::quat q     = glm::quat(angle);
    m_rotation            = glm::toMat4(q);
}

glm::mat4 ITransformable::getModelMatrix() const
{
    return m_position * m_rotation * m_scale;
}
