#pragma once
#include <context.h>
#include <swapchain.h>
#include <vulkan/vulkan_core.h>

namespace ObjectManager {
    void setup(Context& context);
    void draw(VkCommandBuffer cmdBuffer);
    void destroy(VkDevice device);
}