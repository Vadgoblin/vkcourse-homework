#pragma once

// #include <context.h>
#include <glm/fwd.hpp>
#include <vector>
#include <vulkan/vulkan_core.h>
class Context;

class PipelineWrapper {

public:
    PipelineWrapper(Context& context, const VkDevice device, const VkFormat colorFormat, const VkSampleCountFlagBits sampleCountFlagBits);

    void Destroy() const;

    VkPipelineLayout pipelineLayout() const { return m_pipelineLayout; }
    VkPipeline       pipeline() const { return m_pipeline; }
    uint32_t         constantOffset() const { return m_constantOffset; }
    VkDescriptorSetLayout descSetLayout() const { return m_descSetLayout; }

private:
    static VkPipelineLayout CreatePipelineLayout(const VkDevice device, const std::vector<VkDescriptorSetLayout>& layouts,uint32_t pushConstantSize = 0);
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
    VkDescriptorSetLayout m_descSetLayout = VK_NULL_HANDLE;

};
