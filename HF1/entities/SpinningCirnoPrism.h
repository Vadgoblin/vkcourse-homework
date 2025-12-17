#pragma once
#include "BaseEntity.h"
#include "../containers/ObjectGroup.h"

class SpinningCirnoPrism : public BaseEntity {
public:
    SpinningCirnoPrism();
    ~SpinningCirnoPrism() override;
    void draw(VkCommandBuffer cmdBuffer, bool lightintPass, const glm::mat4& parentModel = glm::mat4(1.0f) ) override;
    void create(Context& context, LightningPass& lightningPass, ShadowPass& shadowPass) override;
    void destroy(VkDevice device) override;
    void tick() override;

private:
    float        m_time = 0.0f;
    float        m_speed = 1.0f;
    ObjectGroup* m_objectGroup;
};