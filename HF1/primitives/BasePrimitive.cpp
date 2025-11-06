#include "BasePrimitive.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

BasePrimitive::BasePrimitive(const bool wireframe)
{
     this->wireframe = wireframe;
}


void BasePrimitive::setScale(const float x, const float y, const float z)
{
    m_scale =  glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void BasePrimitive::setPosition(const float x, const float y, const float z)
{
    m_position = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void BasePrimitive::setRotation(float rx, float ry, float rz)
{
    rx = glm::radians(rx);
    ry = glm::radians(ry);
    rz = glm::radians(rz);
    const glm::vec3 angle = glm::vec3(rx, ry, rz);
    const glm::quat q = glm::quat(angle);
    m_rotation = glm::toMat4(q);
}
