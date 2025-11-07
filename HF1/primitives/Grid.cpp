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


void buildGrid(float width, float depth, int rows, int cols,
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    rows = std::max(2, rows);
    cols = std::max(2, cols);

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;
    float dx = width / (cols - 1);
    float dz = depth / (rows - 1);

    // Generate vertices, normals, and texCoords
    for (int i = 0; i < rows; ++i)
    {
        float z = i * dz - halfDepth;
        for (int j = 0; j < cols; ++j)
        {
            float x = j * dx - halfWidth;

            // Vertex position
            vertices.push_back(x);
            vertices.push_back(0.0f); // y-coordinate for flat grid
            vertices.push_back(z);

            // Normal pointing up
            normals.push_back(0.0f);
            normals.push_back(1.0f);
            normals.push_back(0.0f);

            // Texture coordinates
            texCoords.push_back((float)j / (cols - 1));
            texCoords.push_back((float)i / (rows - 1));
        }
    }

    // Generate indices (two triangles per quad)
    for (int i = 0; i < rows - 1; ++i)
    {
        for (int j = 0; j < cols - 1; ++j)
        {
            unsigned int start = i * cols + j;

            // Triangle 1
            indices.push_back(start);
            indices.push_back(start + cols);
            indices.push_back(start + 1);

            // Triangle 2
            indices.push_back(start + 1);
            indices.push_back(start + cols);
            indices.push_back(start + cols + 1);
        }
    }
}
}

Grid::Grid(float width, float depth, int rows, int cols, bool wireframe)
{
    this->wireframe = wireframe;
    this->m_width = width;
    this->m_depth = depth;
    this->m_rows = rows;
    this->m_cols = cols;
}

VkResult Grid::Create(const Context& context,
                      const VkFormat colorFormat,
                      const uint32_t pushConstantStart)
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
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    buildGrid(m_width, m_depth, m_rows,m_cols, vertexData, normals,texCoords, indices);
    // buildGrid(1, 1, 1,1, vertexData, normals,texCoords, indices);
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