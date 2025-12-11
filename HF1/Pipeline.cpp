#include "Pipeline.h"

#include "ModelPushConstant.h"
#include "PipelineUtils.h"
#include "camera.h"
#include "shaders/triangle_in.frag_include.h"
#include "shaders/triangle_in.vert_include.h"

#include <wrappers.h>

Pipeline::Pipeline(const VkDevice device, const VkFormat swapchainFormat, const VkSampleCountFlagBits sampleCountFlagBits)
{
    m_vkDevice = device;

    const uint32_t* m_shaderVertData = SPV_triangle_in_vert;
    size_t m_shaderVertSize = sizeof(SPV_triangle_in_vert);
    const uint32_t* m_shaderFragData = SPV_triangle_in_frag;
    size_t m_shaderFragSize = sizeof(SPV_triangle_in_frag);

    const VkShaderModule shaderVertex   = CreateShaderModule(device, m_shaderVertData, m_shaderVertSize);
    const VkShaderModule shaderFragment = CreateShaderModule(device, m_shaderFragData, m_shaderFragSize);

    m_constantOffset = sizeof(Camera::CameraPushConstant);
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, swapchainFormat, m_pipelineLayout, shaderVertex, shaderFragment, sampleCountFlagBits);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);
}

void Pipeline::Destroy() const
{
    vkDestroyPipeline(m_vkDevice, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_vkDevice, m_pipelineLayout, nullptr);
}
