#include "cylinder.h"

#include <cstdint>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "context.h"
#include "wrappers.h"

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




VkPipelineLayout CreateEmptyPipelineLayout(const VkDevice device, uint32_t pushConstantSize = 0)
{
    const VkPushConstantRange pushConstantRange = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset     = 0,
        .size       = pushConstantSize,
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = 0,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = (pushConstantSize > 0) ? 1u : 0u,
        .pPushConstantRanges    = &pushConstantRange,
    };

    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkResult         result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout);
    assert(result == VK_SUCCESS);

    return layout;
}

VkPipeline CreateSimplePipeline(const VkDevice         device,
                                const VkFormat         colorFormat,
                                const VkPipelineLayout pipelineLayout,
                                const VkShaderModule   shaderVertex,
                                const VkShaderModule   shaderFragment)
{
    // shader stages
    const VkPipelineShaderStageCreateInfo shaders[] = {
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = VK_SHADER_STAGE_VERTEX_BIT,
            .module              = shaderVertex,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        },
        {
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = 0,
            .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module              = shaderFragment,
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        },
    };

    const VkVertexInputBindingDescription bindingInfo = {
        .binding   = 0,
        .stride    = sizeof(float) * 3,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    const VkVertexInputAttributeDescription attributeInfo = {
        .location = 0,
        .binding  = 0,
        .format   = VK_FORMAT_R32G32B32_SFLOAT,
        .offset   = 0u,
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = 0,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 1u,
        .pVertexBindingDescriptions      = &bindingInfo,
        .vertexAttributeDescriptionCount = 1u,
        .pVertexAttributeDescriptions    = &attributeInfo,
    };

    // input assembly
    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    // viewport info
    const VkPipelineViewportStateCreateInfo viewportInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr, // Dynamic state
        .scissorCount  = 1,
        .pScissors     = nullptr, // Dynamic state
    };

    // rasterization info
    // Experiments:
    //  1) Switch polygon mode to "wireframe".
    //  2) Switch cull mode to front.
    //  3) Switch front face mode to clockwise.
    const VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_NONE,
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, // Disabled
        .depthBiasClamp          = 0.0f, // Disabled
        .depthBiasSlopeFactor    = 0.0f, // Disabled
        .lineWidth               = 1.0f,
    };

    // multisample
    const VkPipelineMultisampleStateCreateInfo multisampleInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    // depth stencil
    // "empty" stencil Op state
    const VkStencilOpState emptyStencilOp = {
        .failOp      = VK_STENCIL_OP_KEEP,
        .passOp      = VK_STENCIL_OP_KEEP,
        .depthFailOp = VK_STENCIL_OP_KEEP,
        .compareOp   = VK_COMPARE_OP_NEVER,
        .compareMask = 0,
        .writeMask   = 0,
        .reference   = 0,
    };

    const VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = emptyStencilOp,
        .back                  = emptyStencilOp,
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };

    // color blend
    const VkPipelineColorBlendAttachmentState blendAttachment = {
        .blendEnable = VK_FALSE,
        // as blend is disabled fill these with default values,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR,
        .dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        // Important!
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp       = VK_LOGIC_OP_CLEAR, // Disabled
        // Important!
        .attachmentCount = 1,
        .pAttachments    = &blendAttachment,
        .blendConstants  = {1.0f, 1.0f, 1.0f, 1.0f}, // Ignored
    };

    const VkPipelineRenderingCreateInfo renderingInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &colorFormat,
        .depthAttachmentFormat   = VK_FORMAT_D32_SFLOAT,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    const std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    const VkPipelineDynamicStateCreateInfo dynamicInfo = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0u,
        .dynamicStateCount = (uint32_t)dynamicStates.size(),
        .pDynamicStates    = dynamicStates.data(),
    };

    // pipeline create
    const VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &renderingInfo,
        .flags               = 0,
        .stageCount          = 2,
        .pStages             = shaders,
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState   = &multisampleInfo,
        .pDepthStencilState  = &depthStencilInfo,
        .pColorBlendState    = &colorBlendInfo,
        .pDynamicState       = &dynamicInfo,
        .layout              = pipelineLayout,
        .renderPass          = VK_NULL_HANDLE,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = 0,
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    VkResult   result   = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
    assert(result == VK_SUCCESS);

    return pipeline;
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
