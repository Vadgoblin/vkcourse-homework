#include "LightningPass.h"

#include "camera.h"

#include "glm_config.h"
#include <vector>
#include <vulkan/vulkan_core.h>

#include "ModelPushConstant.h"
#include "primitives/BasePrimitive.h"
#include "shaders/shader.frag_include.h"
#include "shaders/shader.vert_include.h"
#include <wrappers.h>

static VkPipelineLayout CreatePipelineLayout(const VkDevice device, const std::vector<VkDescriptorSetLayout>& layouts, uint32_t pushConstantSize)
{
    const VkPushConstantRange pushConstantRange = {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = pushConstantSize,
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = (uint32_t)layouts.size(),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = (pushConstantSize > 0) ? 1u : 0u,
        .pPushConstantRanges    = &pushConstantRange,
    };

    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkResult         result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &layout);
    assert(result == VK_SUCCESS);

    return layout;
}

static VkPipeline CreatePipeline(const VkDevice         device,
                                 const VkPipelineLayout pipelineLayout,
                                 const VkFormat         colorFormat,
                                 // const VkFormat         depthFormat,
                                 const VkSampleCountFlagBits vkSampleCountFlagBits)
{
    const uint32_t* m_shaderVertData = SPV_shader_in_vert;
    size_t          m_shaderVertSize = sizeof(SPV_shader_in_vert);
    const uint32_t* m_shaderFragData = SPV_shader_in_frag;
    size_t          m_shaderFragSize = sizeof(SPV_shader_in_frag);

    const VkShaderModule shaderVertex   = CreateShaderModule(device, m_shaderVertData, m_shaderVertSize);
    const VkShaderModule shaderFragment = CreateShaderModule(device, m_shaderFragData, m_shaderFragSize);

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

    // buffer binding
    VkVertexInputBindingDescription bindingDescriptions[3] = {
        { 0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX }, // Binding 0: Position
        { 1, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX }, // Binding 1: UV
        { 2, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX }  // Binding 2: Normal
    };

    VkVertexInputAttributeDescription vertexAttributes[3] = {
        { // position
            .location = 0,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = 0,
        },
        { // uv
            .location = 1,
            .binding  = 1,
            .format   = VK_FORMAT_R32G32_SFLOAT,
            .offset   = 0,
        },
        { // normal
            .location = 2,
            .binding  = 2,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = 0,
        }
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = 0,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 3u,
        .pVertexBindingDescriptions      = bindingDescriptions,
        .vertexAttributeDescriptionCount = 3u,
        .pVertexAttributeDescriptions    = vertexAttributes,
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
        .rasterizationSamples  = vkSampleCountFlagBits,
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
    VkPipelineColorBlendAttachmentState blendAttachment = {
        .blendEnable         = VK_TRUE,
        // as blend is disabled fill these with default values,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, //VK_BLEND_FACTOR_ZERO, //ONE, //DST_COLOR,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, //SRC_ALPHA, //SRC_ALPHA, //VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, //VK_BLEND_FACTOR_DST_ALPHA, //ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        // Important!
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
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
        // .pNext               = nullptr,
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

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    return pipeline;
}

LightningPass::LightningPass(Context &context,
                             const VkDevice              device,
                             const VkFormat              colorFormat,
                             const VkSampleCountFlagBits sampleCountFlagBits)
{
    m_vkDevice = device;
    m_vertexDataDescSetLayout = BasePrimitive::CreateVertexDataDescSetLayout(context);
    auto texturedsetlayout = context.textureManager().DescriptorSetLayout();

    m_constantOffset = sizeof(Camera::CameraPushConstant);
    m_pipelineLayout = CreatePipelineLayout(device, {texturedsetlayout, m_vertexDataDescSetLayout}, m_constantOffset + sizeof(ModelPushConstant));
    m_pipeline = CreatePipeline(device, m_pipelineLayout, colorFormat, sampleCountFlagBits);
}
void LightningPass::Destroy() const
{
    vkDestroyPipeline(m_vkDevice, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_vkDevice, m_pipelineLayout, nullptr);
}
