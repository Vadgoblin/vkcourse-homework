#include "Cube.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "context.h"
#include "wrappers.h"
#include "../PipelineUtils.h"

namespace {

#include "triangle_in.frag_include.h"
#include "triangle_in.vert_include.h"

static constexpr float g_cubeVertices[] = {
#include "04_cube_vertices.inc"
};
static constexpr size_t g_cubePerVertexItemCount = 5;
static constexpr size_t g_cubeVertexSize         = sizeof(float) * g_cubePerVertexItemCount;
static constexpr size_t g_cubeVertexCount        = sizeof(g_cubeVertices) / g_cubeVertexSize;
} // anonymous namespace

Cube::Cube(bool wireframe)
{
    this->wireframe = wireframe;
}

VkResult Cube::Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart)
{
    const VkDevice       device       = context.device();
    const VkShaderModule shaderVertex = CreateShaderModule(device, SPV_triangle_in_vert, sizeof(SPV_triangle_in_vert));
    const VkShaderModule shaderFragment =
        CreateShaderModule(device, SPV_triangle_in_frag, sizeof(SPV_triangle_in_frag));

    m_constantOffset = pushConstantStart;
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, colorFormat, m_pipelineLayout, shaderVertex, shaderFragment, context.sampleCountFlagBits() , this->wireframe);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    // Helper class to represent a single vertex
    struct Vertex {
        float x;
        float y;
        float z;
    };
    std::vector<Vertex> vertexData;
    vertexData.resize(g_cubeVertexCount);
    for (size_t idx = 0; idx < g_cubeVertexCount; ++idx) {
        const size_t position = idx * g_cubePerVertexItemCount;
        vertexData[idx]       = {g_cubeVertices[position], g_cubeVertices[position + 1], g_cubeVertices[position + 2]};
    }
    m_vertexCount = vertexData.size();

    const uint32_t vertexDataSize = vertexData.size() * sizeof(vertexData[0]);
    m_buffer = BufferInfo::Create(context.physicalDevice(), device, vertexDataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    // Upload buffer data
    void* dataPtr = m_buffer.Map(device);
    memcpy(dataPtr, vertexData.data(), vertexDataSize);
    m_buffer.Unmap(device);

    return VK_SUCCESS;
}

void Cube::Destroy(const VkDevice device)
{
    m_buffer.Destroy(device);
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void Cube::Draw(const VkCommandBuffer cmdBuffer)
{
    const ModelPushConstant modelData = {
        .model = m_position * m_rotation * m_scale,
    };

    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkDeviceSize nullOffset = 0u;
    vkCmdBindVertexBuffers(cmdBuffer, 0u, 1u, &m_buffer.buffer, &nullOffset);
    vkCmdDraw(cmdBuffer, m_vertexCount, 1, 0, 0);
}
