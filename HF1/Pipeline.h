#pragma once

#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>

class Pipeline {

public:
    explicit Pipeline(VkDevice device, VkFormat swapchainFormat,VkSampleCountFlagBits sampleCountFlagBits);

    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         constantOffset() const { return m_constantOffset; }

private:
    VkDevice    m_vkDevice;
    VkPipelineLayout  m_pipelineLayout;
    VkPipeline        m_pipeline;
    glm::uint32_t     m_constantOffset = 0;

};
