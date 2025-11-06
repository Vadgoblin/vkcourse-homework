#pragma once
#include <vulkan/vulkan.h>

VkPipelineLayout CreateEmptyPipelineLayout(VkDevice device, uint32_t pushConstantSize);
VkPipeline CreateSimplePipeline(VkDevice device,
                                VkFormat colorFormat,
                                VkPipelineLayout layout,
                                VkShaderModule vertShader,
                                VkShaderModule fragShader,
                                VkSampleCountFlagBits vkSampleCountFlagBits,
                                bool wireframe = false);