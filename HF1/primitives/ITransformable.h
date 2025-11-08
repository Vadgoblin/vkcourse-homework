#pragma once
#include "glm/fwd.hpp"
#include "glm_config.h"

class ITransformable {
public:
    virtual ~ITransformable() = default;

    void setScale(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setRotation(float rx, float ry, float rz);
    glm::mat4 getModelMatrix() const;

private:
    glm::mat4        m_scale          = glm::mat4(1.0f);
    glm::mat4        m_position       = glm::mat4(1.0f);
    glm::mat4        m_rotation       = glm::mat4(1.0f);
};