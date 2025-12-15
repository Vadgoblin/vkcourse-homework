#pragma once
#include <vulkan/vulkan_core.h>
#include "glm_config.h"

class LightningPass {
public:
    LightningPass(VkDevice device,
                  VkFormat colorFormat,
                  VkSampleCountFlagBits sampleCountFlagBits,
                  VkDescriptorSetLayout descSetLayout);
    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         constantOffset() const { return m_constantOffset; }

private:
    VkDevice    m_vkDevice;
    VkPipelineLayout  m_pipelineLayout;
    VkPipeline        m_pipeline;
    glm::uint32_t     m_constantOffset = 0;
    VkDescriptorSetLayout m_descSetLayout = VK_NULL_HANDLE;
};
