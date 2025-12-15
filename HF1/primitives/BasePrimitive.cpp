#include "BasePrimitive.h"
#include "../shared_crap.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../ModelPushConstant.h"
#include "context.h"
#include <texture.h>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace {
#include "shaders/shader.frag_include.h"
#include "shaders/shader.vert_include.h"
}

BasePrimitive::BasePrimitive()
{
     m_shaderVertData = SPV_shader_in_vert;
     m_shaderVertSize = sizeof(SPV_shader_in_vert);
     m_shaderFragData = SPV_shader_in_frag;
     m_shaderFragSize = sizeof(SPV_shader_in_frag);
}

VkResult BasePrimitive::create(Context& context, const char* texture_name)
{
    m_pipeline = context.lightning_pass().pipeline();
    m_pipelineLayout = context.lightning_pass().pipelineLayout();
    m_constantOffset = context.lightning_pass().constantOffset();
    m_vertexCount = static_cast<uint32_t>(m_indices.size());

    m_vertexBuffer = UploadToGPU(context, m_vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_texCoordBuffer = UploadToGPU(context, m_texCoords, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_indexBuffer = UploadToGPU(context, m_indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    m_normalBuffer = UploadToGPU(context, m_normals, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);


    Texture *texture = context.texture_manager().GetTexture(texture_name);

    m_modelSet = context.descriptorPool().CreateSet(descSetLayout);

    DescriptorSetMgmt setMgmt(m_modelSet);
    setMgmt.SetImage(0, texture->view(), texture->sampler(),VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    setMgmt.Update(context.device());

    return VK_SUCCESS;
}

void BasePrimitive::destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
    m_texCoordBuffer.Destroy(device);
    m_normalBuffer.Destroy(device);
}

void BasePrimitive::draw(const VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    const ModelPushConstant modelData = {
        .model = parentModel * getModelMatrix(),
    };

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);

    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_modelSet, 0,
                            nullptr);

    VkBuffer     vertexBuffers[] = {m_vertexBuffer.buffer, m_texCoordBuffer.buffer, m_normalBuffer.buffer};
    VkDeviceSize offsets[]       = {0, 0, 0};
    vkCmdBindVertexBuffers(cmdBuffer, 0, 3, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}