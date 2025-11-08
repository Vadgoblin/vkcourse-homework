#pragma once
#include "BaseEntity.h"
#include "../containers/ObjectGroup.h"

class PistonWithBouncingBall : public BaseEntity {
public:
    PistonWithBouncingBall();
    ~PistonWithBouncingBall();
    void draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f)) override;
    void create(const Context& context, VkFormat colorFormat, uint32_t pushConstantStart) override;
    void destroy(VkDevice device) override;
    void tick() override;

private:
    const float PI = 3.14159265359f;
    float        m_animationProgress = 0.0f;
    float        m_speed = 0.075f;

    ObjectGroup* m_pistonBase;
    ObjectGroup* m_pistonMovingPart;
    ObjectGroup* m_ball;
};