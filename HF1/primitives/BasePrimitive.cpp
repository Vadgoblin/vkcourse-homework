#include "BasePrimitive.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../render_passes/ShadowPass.h"
#include "../managers/TextureManager.h"
#include "context.h"

#include <texture.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "../render_passes/LightningPass.h"

VkResult BasePrimitive::create(Context& context,LightningPass& lightningPass, ShadowPass& shadowPass,  const char* texture_name)
{
    m_lightningPassPipeline = lightningPass.pipeline();
    m_lightningPassPipelineLayout = lightningPass.pipelineLayout();
    m_lightningPassConstantOffset = lightningPass.modelPushConstantOffset();

    m_shadowPassPipeline = shadowPass.pipeline();
    m_shadowPassPipelineLayout = shadowPass.pipelineLayout();
    m_shadowPassConstantOffset = shadowPass.modelPushConstantOffset();

    m_vertexCount = static_cast<uint32_t>(m_indices.size());

    m_vertexBuffer = UploadToGPU(context, m_vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_texCoordBuffer = UploadToGPU(context, m_texCoords, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    m_indexBuffer = UploadToGPU(context, m_indices, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    m_normalBuffer = UploadToGPU(context, m_normals, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);


    Texture *texture = lightningPass.textureManager().GetTexture(texture_name);

    m_modelSet = context.descriptorPool().CreateSet(lightningPass.textureManager().DescriptorSetLayout());// !

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

void BasePrimitive::draw(const VkCommandBuffer cmdBuffer,bool lightningPass, const glm::mat4& parentModel)
{
    const ModelPushConstant modelData = {
        .model = parentModel * getModelMatrix(),
    };

    if (lightningPass) {
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightningPassPipeline);
        vkCmdPushConstants(cmdBuffer, m_lightningPassPipelineLayout, VK_SHADER_STAGE_ALL, m_lightningPassConstantOffset,
                           sizeof(ModelPushConstant), &modelData);

        vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_lightningPassPipelineLayout, 0, 1, &m_modelSet, 0,
                                nullptr);

        VkBuffer     vertexBuffers[] = {m_vertexBuffer.buffer, m_texCoordBuffer.buffer, m_normalBuffer.buffer};
        VkDeviceSize offsets[]       = {0, 0, 0};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 3, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
    }
    else {
        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shadowPassPipeline);
        vkCmdPushConstants(cmdBuffer, m_shadowPassPipelineLayout, VK_SHADER_STAGE_ALL, m_shadowPassConstantOffset,
                           sizeof(ModelPushConstant), &modelData);

        VkBuffer     vertexBuffers[] = {m_vertexBuffer.buffer};
        VkDeviceSize offsets[]       = {0};
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
    }
}