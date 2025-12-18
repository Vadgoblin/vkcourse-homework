#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

void PrintPhyDeviceInfo(VkInstance /*instance*/, VkPhysicalDevice phyDevice);

VkResult                     CreateCommandPool(VkDevice device, uint32_t queueFamilyIdx, VkCommandPool* outCmdPool);
std::vector<VkCommandBuffer> AllocateCommandBuffers(VkDevice device, VkCommandPool cmdPool, uint32_t count);

VkFence CreateFence(VkDevice device);

VkSemaphore CreateSemaphore(VkDevice device);

VkShaderModule CreateShaderModule(VkDevice device, const uint32_t* SPIRVBinary, uint32_t SPIRVBinarySize);

VkPipelineLayout
CreatePipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& layouts, uint32_t pushConstantSize = 0);
