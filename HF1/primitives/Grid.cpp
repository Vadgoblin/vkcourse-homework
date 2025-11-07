//
// Created by goblin on 2025.11.07..
//

#include "Grid.h"

#include <cstdio>

#include <vector>
#include <vulkan/vulkan_core.h>

#include "../PipelineUtils.h"
#include "buffer.h"
#include "context.h"
#include "descriptors.h"
#include "wrappers.h"

#include <cstring>

namespace {
// #include "triangle_in.frag_include.h"
// #include "triangle_in.vert_include.h"

#include "grid.frag_include.h"
#include "grid.vert_include.h"


void buildGrid(float    width,
               float    height,
               uint32_t count,
               std::vector<float>&        vertices,
               std::vector<unsigned int>& indices   )
{
    float halfWidth  = width / 2.0f;
    float halfHeight = height / 2.0f;

    for (uint32_t y = 0; y <= count; y++) {
        for (uint32_t x = 0; x <= count; x++) {
            vertices.push_back((float)x / count * width - halfWidth);
            vertices.push_back((float)y / count * height - halfHeight);
            vertices.push_back(0.0f);
            // u = (float)x / count,
            // v = (float)y / count,
        }
    }

    for (uint32_t y = 0; y < count; y++) {
        for (uint32_t x = 0; x < count; x++) {
            uint32_t row     = y * (count + 1);
            uint32_t rowNext = (y + 1) * (count + 1);

            indices.push_back(row + x);
            indices.push_back(row + x + 1);
            indices.push_back(rowNext + x + 1);

            indices.push_back(row + x);
            indices.push_back(rowNext + x + 1);
            indices.push_back(rowNext + x);
        }
    }
}
}

Grid::Grid()
= default;

VkResult Grid::Create(const Context& context,
                      const VkFormat colorFormat,
                      const uint32_t pushConstantStart,
                      float          width,
                      float          height,
                      uint32_t       count)
{
    const VkDevice       device       = context.device();
    const VkShaderModule shaderVertex = CreateShaderModule(device, SPV_grid_vert, sizeof(SPV_grid_vert));
    const VkShaderModule shaderFragment =
        CreateShaderModule(device, SPV_grid_frag, sizeof(SPV_grid_frag));

    m_constantOffset = pushConstantStart;
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, colorFormat, m_pipelineLayout, shaderVertex, shaderFragment, context.sampleCountFlagBits() ,this->wireframe);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    std::vector<float> vertexData;
    std::vector<unsigned int> indices;

    buildGrid(width, height, count, vertexData,  indices);
    m_vertexCount = static_cast<uint32_t>(indices.size());

    const uint32_t vertexDataSize = vertexData.size() * sizeof(float);
    m_vertexBuffer = BufferInfo::Create(context.physicalDevice(), device, vertexDataSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    void* dataPtr = m_vertexBuffer.Map(device);
    memcpy(dataPtr, vertexData.data(), vertexDataSize);
    m_vertexBuffer.Unmap(device);

    const uint32_t indexDataSize = indices.size() * sizeof(uint32_t);
    m_indexBuffer = BufferInfo::Create(context.physicalDevice(), device, indexDataSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
    dataPtr = m_indexBuffer.Map(device);
    memcpy(dataPtr, indices.data(), indexDataSize);
    m_indexBuffer.Unmap(device);

    return VK_SUCCESS;
}


void Grid::Destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void Grid::Draw(const VkCommandBuffer cmdBuffer)
{
    const ModelPushConstant modelData = {
        .model = m_position * m_rotation * m_scale,
    };

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);


    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}