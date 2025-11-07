#include "Sphere.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "context.h"
#include "wrappers.h"
#include "../PipelineUtils.h"

namespace {
#include "triangle_in.frag_include.h"
#include "triangle_in.vert_include.h"

}

void buildSphere(float                      radius,
                 int                        sectorCount,
                 int                        stackCount,
                 std::vector<float>&        vertices,
                 std::vector<float>&        normals,
                 std::vector<float>&        texCoords,
                 std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;

    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();

    float x, y, z, xy;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // normal
    float s, t;                                  // texCoords

    float sectorStep = 2 * PI / sectorCount;
    float stackStep  = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;    // from +pi/2 to ‑pi/2
        xy         = radius * cosf(stackAngle); // r * cos(u)
        z          = radius * sinf(stackAngle); // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep; // from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // texture coordinate (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // indices
    // k1: first vertex index of current stack
    // k2: first vertex index of next stack
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1); // beginning of current stack
        int k2 = k1 + sectorCount + 1;  // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector except for first/last stacks
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

Sphere::Sphere(float radius, int sectorCount, int stackCount, bool wireframe)
{
    this->m_radius = radius;
    this->m_sectorCount = sectorCount;
    this->m_stackCount = stackCount;
    this->wireframe = wireframe;
}

VkResult Sphere::Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart)
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

    buildSphere(this->m_radius,this->m_sectorCount,this->m_stackCount, vertexData,normals,texCoords,indices);
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

void Sphere::Destroy(const VkDevice device)
{
    m_vertexBuffer.Destroy(device);
    m_indexBuffer.Destroy(device);
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void Sphere::Draw(const VkCommandBuffer cmdBuffer)
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