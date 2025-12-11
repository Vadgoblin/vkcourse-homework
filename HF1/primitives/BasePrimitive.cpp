#include "BasePrimitive.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <cstring>
#include <wrappers.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "context.h"
#include "../ModelPushConstant.h"

namespace {
#include "shaders/triangle_in.frag_include.h"
#include "shaders/triangle_in.vert_include.h"
}

BasePrimitive::BasePrimitive()
{
     m_shaderVertData = SPV_triangle_in_vert;
     m_shaderVertSize = sizeof(SPV_triangle_in_vert);
     m_shaderFragData = SPV_triangle_in_frag;
     m_shaderFragSize = sizeof(SPV_triangle_in_frag);
}

VkResult BasePrimitive::create(const Context& context)
{
    m_pipeline = context.pipeline().pipeline();
    m_pipelineLayout = context.pipeline().pipelineLayout();
    m_constantOffset = context.pipeline().constantOffset();

    m_vertexCount = static_cast<uint32_t>(m_indices.size());

    const uint32_t vertexDataSize = m_vertices.size() * sizeof(float);
    m_vertexBuffer = BufferInfo::Create(context.physicalDevice(), context.device(), vertexDataSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    void* dataPtr = m_vertexBuffer.Map(context.device());
    memcpy(dataPtr, m_vertices.data(), vertexDataSize);
    m_vertexBuffer.Unmap(context.device());

    const uint32_t texCoordDataSize = m_texCoords.size() * sizeof(float);
    m_texCoordBuffer = BufferInfo::Create(context.physicalDevice(), context.device(), texCoordDataSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    dataPtr = m_texCoordBuffer.Map(context.device());
    memcpy(dataPtr, m_texCoords.data(), vertexDataSize);
    m_texCoordBuffer.Unmap(context.device());

    const uint32_t indexDataSize = m_indices.size() * sizeof(uint32_t);
    m_indexBuffer = BufferInfo::Create(context.physicalDevice(), context.device(), indexDataSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    dataPtr = m_indexBuffer.Map(context.device());
    memcpy(dataPtr, m_indices.data(), indexDataSize);
    m_indexBuffer.Unmap(context.device());

    return VK_SUCCESS;
}

void BasePrimitive::destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
    m_texCoordBuffer.Destroy(device);
}

void BasePrimitive::draw(const VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    const ModelPushConstant modelData = {
        .model = parentModel * getModelMatrix(),
    };

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);

    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer,m_texCoordBuffer.buffer };
    VkDeviceSize offsets[] = { 0,0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 2,  vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}
