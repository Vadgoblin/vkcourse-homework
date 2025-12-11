#pragma once
#include <context.h>
#include <swapchain.h>
#include <vulkan/vulkan_core.h>

namespace ObjectManager {
    void setup(const Context& context, const Swapchain& swapchain, size_t pushConstansStart, VkPipeline pipeline );
    void draw(VkCommandBuffer cmdBuffer);
    void destroy(VkDevice device);
}