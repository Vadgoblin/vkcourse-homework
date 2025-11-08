#pragma once
#include <glm/fwd.hpp>
#include <glm_config.h>
#include <vulkan/vulkan_core.h>

class IDrawable {
public:
    virtual void draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f)) = 0;
    virtual ~IDrawable() = default;
};