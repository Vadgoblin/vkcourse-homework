#pragma once
#include "glm_config.h"
#include <vulkan/vulkan_core.h>

#include <vector>

class Context;
class LightningPass {
public:
    LightningPass(Context &context,VkDevice device,
                  VkFormat colorFormat,
                  VkSampleCountFlagBits sampleCountFlagBits);
    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         modelPushConstantOffset() const { return m_modelPushConstantOffset; }

private:
    VkDevice              m_vkDevice;
    VkPipelineLayout      m_pipelineLayout;
    VkPipeline            m_pipeline;
    glm::uint32_t         m_modelPushConstantOffset;
    VkDescriptorSetLayout m_vertexDataDescSetLayout;
};
