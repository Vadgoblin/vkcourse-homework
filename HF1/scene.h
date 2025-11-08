#pragma once
#include <context.h>
#include <swapchain.h>
#include <vulkan/vulkan_core.h>

namespace ObjectManager {
    void SetupAll(const Context& context, const Swapchain& swapchain, size_t pushConstansStart);
    void DrawAll(VkCommandBuffer cmdBuffer);
    void DestroyAll(VkDevice device);
}