#pragma once

#include <glm/fwd.hpp>
#include <vulkan/vulkan_core.h>

class PipelineWrapper {

public:
    explicit PipelineWrapper(VkDevice device, VkFormat swapchainFormat,VkSampleCountFlagBits sampleCountFlagBits);

    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         constantOffset() const { return m_constantOffset; }

private:
    static VkPipelineLayout CreateEmptyPipelineLayout(const VkDevice device, uint32_t pushConstantSize = 0);
    static VkPipeline CreateSimplePipeline(const VkDevice         device,
                                    const VkFormat         colorFormat,
                                    const VkPipelineLayout pipelineLayout,
                                    const VkShaderModule   shaderVertex,
                                    const VkShaderModule   shaderFragment,
                                    const VkSampleCountFlagBits vkSampleCountFlagBits);

    VkDevice    m_vkDevice;
    VkPipelineLayout  m_pipelineLayout;
    VkPipeline        m_pipeline;
    glm::uint32_t     m_constantOffset = 0;

};
