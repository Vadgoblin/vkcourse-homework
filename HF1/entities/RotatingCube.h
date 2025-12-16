#pragma once
#include "BaseEntity.h"
#include "../containers/ObjectGroup.h"


class RotatingCube final: public BaseEntity{
public:
    RotatingCube();
    void draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f)) override;
    void create(Context& context, LightningPass& lightningPass) override;
    void destroy(VkDevice device) override;
    void tick() override;

private:
    float        m_time = 0.0f;
    float        m_speed = 1.0f;
    ObjectGroup* m_objectGroup;
};