#include "cylinder.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "context.h"
#include "wrappers.h"
#include "PipelineUtils.h"

namespace {

#include "triangle_in.frag_include.h"
#include "triangle_in.vert_include.h"


// Generate a cylinder mesh (smooth) with base radius, top radius (for truncated),
// height, number of sectors (slices around) and stacks (segments along height).
void buildCylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int sectorCount,
                   int stackCount,
                   std::vector<float>& vertices,
                   std::vector<float>& normals,
                   std::vector<float>& texCoords,
                   std::vector<unsigned int>& indices)
{
    const float PI = 3.14159265359f;
    float sectorStep = 2.0f * PI / sectorCount;
    float stackStep = height / stackCount;
    float radiusStep = (topRadius - baseRadius) / stackCount;

    // Vertices, normals, texCoords
    for(int i = 0; i <= stackCount; ++i)
    {
        float curRadius = baseRadius + i * radiusStep;
        float z = -0.5f * height + i * stackStep;    // centered about origin
        for(int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = j * sectorStep;
            float x = curRadius * cosf(sectorAngle);
            float y = curRadius * sinf(sectorAngle);

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal (for side surface)
            float nx = cosf(sectorAngle);
            float ny = sinf(sectorAngle);
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(0.0f);

            // texture coord
            float s = (float)j / (float)sectorCount;
            float t = (float)i / (float)stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    // Indices for side surfaces
    for(int i = 0; i < stackCount; ++i)
    {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // triangle 1
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);

            // triangle 2
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
        }
    }
}






} // anonymous namespace

Cylinder::Cylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int sectorCount,
                   int stackCount,
                   bool wireframe)
    : m_pipelineLayout(VK_NULL_HANDLE)
    , m_pipeline(VK_NULL_HANDLE)
{
    this->baseRadius = baseRadius;
    this->topRadius = topRadius;
    this->height = height;
    this->sectorCount = sectorCount;
    this->stackCount = stackCount;
    this->wireframe = wireframe;
}

VkResult Cylinder::Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart)
{
    const VkDevice       device       = context.device();
    const VkShaderModule shaderVertex = CreateShaderModule(device, SPV_triangle_in_vert, sizeof(SPV_triangle_in_vert));
    const VkShaderModule shaderFragment =
        CreateShaderModule(device, SPV_triangle_in_frag, sizeof(SPV_triangle_in_frag));

    m_constantOffset = pushConstantStart;
    m_pipelineLayout = CreateEmptyPipelineLayout(device, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline       = CreateSimplePipeline(device, colorFormat, m_pipelineLayout, shaderVertex, shaderFragment);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    std::vector<float> vertexData;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;

    buildCylinder(this->baseRadius,this->topRadius,this->height,this->sectorCount,this->stackCount, vertexData,normals,texCoords,indices);
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

    // const uint32_t vertexDataSize = vertexData.size() * sizeof(vertexData[0]);
    // m_buffer = BufferInfo::Create(context.physicalDevice(), device, vertexDataSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    //
    // // Upload buffer data
    // void* dataPtr = m_buffer.Map(device);
    // memcpy(dataPtr, vertexData.data(), vertexDataSize);
    // m_buffer.Unmap(device);

    return VK_SUCCESS;
}

void Cylinder::Destroy(const VkDevice device)
{
    // m_buffer.Destroy(device);
    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
}

void Cylinder::Draw(const VkCommandBuffer cmdBuffer)
{
    const ModelPushConstant modelData = {
        .model = m_position * m_rotation * m_scale,
    };

    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, m_constantOffset,
                       sizeof(ModelPushConstant), &modelData);
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    // VkDeviceSize nullOffset = 0u;
    // vkCmdBindVertexBuffers(cmdBuffer, 0u, 1u, &m_buffer.buffer, &nullOffset);
    // vkCmdDraw(cmdBuffer, m_vertexCount, 1, 0, 0);



    VkBuffer vertexBuffers[] = { m_vertexBuffer.buffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(cmdBuffer, m_indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, m_vertexCount, 1, 0, 0, 0);
}
void Cylinder::setScale(const float x, const float y, const float z)
{
    m_scale =  glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void Cylinder::setPosition(const float x, const float y, const float z)
{
    m_position = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}
