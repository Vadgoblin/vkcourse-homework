#include "Cube.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "../PipelineUtils.h"
#include "../debug.h"
#include "context.h"
#include "wrappers.h"

namespace {

#include "triangle_in.frag_include.h"
#include "triangle_in.vert_include.h"

} // anonymous namespace

void buildCube(float size,
               std::vector<float>& vertices,
               std::vector<float>& normals,
               std::vector<float>& texCoords,
               std::vector<unsigned int>& indices)
{
    float half = size / 2.0f;

    // 8 cube corners
    float cubeVertices[8][3] = {
        {-half, -half, -half}, {half, -half, -half},
        {half,  half, -half}, {-half,  half, -half},
        {-half, -half,  half}, {half, -half,  half},
        {half,  half,  half}, {-half,  half,  half}
    };

    // cube normals for each face
    float cubeNormals[6][3] = {
        { 0,  0, -1}, // back
        { 0,  0,  1}, // front
        { 0, -1,  0}, // bottom
        { 0,  1,  0}, // top
        {-1,  0,  0}, // left
        { 1,  0,  0}  // right
    };

    // texture coordinates
    float cubeTexCoords[4][2] = {
        {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
    };

    // indices for each face (two triangles per face)
    unsigned int faceIndices[6][4] = {
        {0, 1, 2, 3}, // back
        {4, 5, 6, 7}, // front
        {0, 1, 5, 4}, // bottom
        {3, 2, 6, 7}, // top
        {0, 3, 7, 4}, // left
        {1, 2, 6, 5}  // right
    };

    // Generate vertices, normals, texCoords
    for (int f = 0; f < 6; ++f)
    {
        for (int v = 0; v < 4; ++v)
        {
            int vi = faceIndices[f][v];
            vertices.push_back(cubeVertices[vi][0]);
            vertices.push_back(cubeVertices[vi][1]);
            vertices.push_back(cubeVertices[vi][2]);

            normals.push_back(cubeNormals[f][0]);
            normals.push_back(cubeNormals[f][1]);
            normals.push_back(cubeNormals[f][2]);

            texCoords.push_back(cubeTexCoords[v][0]);
            texCoords.push_back(cubeTexCoords[v][1]);
        }

        // Add indices (two triangles per face)
        unsigned int startIndex = f * 4;
        indices.push_back(startIndex);
        indices.push_back(startIndex + 1);
        indices.push_back(startIndex + 2);

        indices.push_back(startIndex);
        indices.push_back(startIndex + 2);
        indices.push_back(startIndex + 3);
    }
}



Cube::Cube(const float size, const bool wireframe)
{
    this->wireframe = wireframe;
    this->m_size = size;
}

VkResult Cube::Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart)
{
    const VkDevice       device       = context.device();
    const VkShaderModule shaderVertex = CreateShaderModule(device, SPV_triangle_in_vert, sizeof(SPV_triangle_in_vert));
    const VkShaderModule shaderFragment =
        CreateShaderModule(device, SPV_triangle_in_frag, sizeof(SPV_triangle_in_frag));

    m_constantOffset = pushConstantStart;
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, colorFormat, m_pipelineLayout, shaderVertex, shaderFragment, context.sampleCountFlagBits() ,this->wireframe);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    std::vector<float> vertexData;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    buildCube(m_size, vertexData, normals, texCoords, indices);
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

void Cube::Destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
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

    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}
