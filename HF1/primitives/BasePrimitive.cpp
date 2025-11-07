#include "BasePrimitive.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <cstring>
#include <wrappers.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "context.h"
#include "../PipelineUtils.h"

BasePrimitive::BasePrimitive(const bool wireframe)
{
     this->wireframe = wireframe;
}

VkResult BasePrimitive::Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart)
{
    const VkDevice       device         = context.device();
    const VkShaderModule shaderVertex   = CreateShaderModule(device, m_shaderVertData, m_shaderVertSize);
    const VkShaderModule shaderFragment = CreateShaderModule(device, m_shaderFragData, m_shaderFragSize);

    m_constantOffset = pushConstantStart;
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, colorFormat, m_pipelineLayout, shaderVertex, shaderFragment, context.sampleCountFlagBits() ,this->wireframe);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    m_vertexCount = static_cast<uint32_t>(m_indices.size());

    const uint32_t vertexDataSize = m_vertices.size() * sizeof(float);
    m_vertexBuffer = BufferInfo::Create(context.physicalDevice(), device, vertexDataSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    void* dataPtr = m_vertexBuffer.Map(device);
    memcpy(dataPtr, m_vertices.data(), vertexDataSize);
    m_vertexBuffer.Unmap(device);

    const uint32_t indexDataSize = m_indices.size() * sizeof(uint32_t);
    m_indexBuffer = BufferInfo::Create(context.physicalDevice(), device, indexDataSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    dataPtr = m_indexBuffer.Map(device);
    memcpy(dataPtr, m_indices.data(), indexDataSize);
    m_indexBuffer.Unmap(device);

    return VK_SUCCESS;
}

void BasePrimitive::Destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void BasePrimitive::Draw(const VkCommandBuffer cmdBuffer)
{
    const ModelPushConstant modelData = {
        .model = m_position * m_rotation * m_scale,
    };

    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}

void BasePrimitive::setScale(const float x, const float y, const float z)
{
    m_scale =  glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void BasePrimitive::setPosition(const float x, const float y, const float z)
{
    m_position = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void BasePrimitive::setRotation(float rx, float ry, float rz)
{
    rx = glm::radians(rx);
    ry = glm::radians(ry);
    rz = glm::radians(rz);
    const glm::vec3 angle = glm::vec3(rx, ry, rz);
    const glm::quat q = glm::quat(angle);
    m_rotation = glm::toMat4(q);
}
