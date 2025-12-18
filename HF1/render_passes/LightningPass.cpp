#include "LightningPass.h"

#include "ShadowPass.h"
#include "../camera.h"

#include "glm_config.h"
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../managers/TextureManager.h"
#include "../primitives/BasePrimitive.h"
#include "shaders/lightning_pass.frag_include.h"
#include "shaders/lightning_pass.vert_include.h"
#include <wrappers.h>

static VkPipelineLayout CreatePipelineLayout(const VkDevice                            device,
                                             const std::vector<VkDescriptorSetLayout>& layouts,
                                             uint32_t                                  pushConstantSize)
{
    const VkPushConstantRange pushConstantRange = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset     = 0,
        .size       = pushConstantSize,
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = static_cast<uint32_t>(layouts.size()),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = 1u,
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
        {0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX}, // Binding 0: Position
        {1, sizeof(glm::vec2), VK_VERTEX_INPUT_RATE_VERTEX}, // Binding 1: UV
        {2, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX}  // Binding 2: Normal
    };

    VkVertexInputAttributeDescription vertexAttributes[3] = {{
                                                                 // position
                                                                 .location = 0,
                                                                 .binding  = 0,
                                                                 .format   = VK_FORMAT_R32G32B32_SFLOAT,
                                                                 .offset   = 0,
                                                             },
                                                             {
                                                                 // uv
                                                                 .location = 1,
                                                                 .binding  = 1,
                                                                 .format   = VK_FORMAT_R32G32_SFLOAT,
                                                                 .offset   = 0,
                                                             },
                                                             {
                                                                 // normal
                                                                 .location = 2,
                                                                 .binding  = 2,
                                                                 .format   = VK_FORMAT_R32G32B32_SFLOAT,
                                                                 .offset   = 0,
                                                             }};

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
        .blendEnable = VK_TRUE,
        // as blend is disabled fill these with default values,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, // VK_BLEND_FACTOR_ZERO, //ONE, //DST_COLOR,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,  // SRC_ALPHA, //SRC_ALPHA, //VK_BLEND_FACTOR_SRC_ALPHA,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // VK_BLEND_FACTOR_DST_ALPHA, //ZERO,
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
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
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

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    return pipeline;
}

LightningPass::LightningPass(Context&                    context,
                             TextureManager&             textureManager,
                             LightManager&               lightManager,
                             ShadowPass&                 shadowPass,
                             const VkFormat              colorFormat,
                             const VkSampleCountFlagBits msaaLevel,
                             const VkFormat              depthFormat,
                             const VkExtent2D            extent)
    : m_device(context.device())
    , m_phyDevice(context.physicalDevice())
    , m_colorFormat(colorFormat)
    , m_depthFormat(depthFormat)
    , m_extent(extent)
    , m_sampleCountFlagBits(msaaLevel)
    , m_textureManager(textureManager)
    , m_lightManager(lightManager)
    , m_shadowPass(shadowPass)
{
    // const auto vertexDataDescSetLayout = BasePrimitive::CreateVertexDataDescSetLayout(context);
    const auto textureDescSetLayout    = textureManager.DescriptorSetLayout();
    const auto lightDescSetLayout      = lightManager.GetDescriptorSetLayout();
    const auto shadowMapDescSetLayout  = shadowPass.ShadowMapDescSetLayout();

    // vertexDataDescSetLayout,
    const std::vector<VkDescriptorSetLayout> layouts = {textureDescSetLayout,
                                                        lightDescSetLayout, shadowMapDescSetLayout};
    const u_int32_t pushConstantSize = sizeof(Camera::CameraPushConstant) + sizeof(BasePrimitive::ModelPushConstant);

    m_modelPushConstantOffset = sizeof(Camera::CameraPushConstant);
    m_pipelineLayout          = CreatePipelineLayout(m_device, layouts, pushConstantSize);
    m_pipeline                = CreatePipeline(m_device, m_pipelineLayout, colorFormat, m_sampleCountFlagBits);

    m_colorOutput     = Texture::Create2D(m_phyDevice, m_device, m_colorFormat, m_extent,
                                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                          VK_SAMPLE_COUNT_1_BIT);
    m_colorOutputMsaa = Texture::Create2D(m_phyDevice, m_device, m_colorFormat, m_extent,
                                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                          m_sampleCountFlagBits);

    m_depthOutput     = Texture::Create2D(m_phyDevice, m_device, m_depthFormat, m_extent,
                                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    m_depthOutputMsaa = Texture::Create2D(m_phyDevice, m_device, m_depthFormat, m_extent,
                                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                          m_sampleCountFlagBits);
}
void LightningPass::Destroy() const
{
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

    m_colorOutput->Destroy(m_device);
    m_colorOutputMsaa->Destroy(m_device);
    m_depthOutput->Destroy(m_device);
    m_depthOutputMsaa->Destroy(m_device);
}

void LightningPass::BeginPass(const VkCommandBuffer cmdBuffer)
{
    TransitionForRender(cmdBuffer);

    const bool             msaa       = (m_sampleCountFlagBits != VK_SAMPLE_COUNT_1_BIT);
    constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    const VkImageView targetView = msaa ? m_colorOutputMsaa->view() : m_colorOutput->view();

    const VkImageView resolveView = msaa ? m_colorOutput->view() : VK_NULL_HANDLE;

    const VkResolveModeFlagBits resolveMode = msaa ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE;

    const VkImageLayout resolveLayout = msaa ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;

    VkRenderingAttachmentInfoKHR colorAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext              = nullptr,
        .imageView          = targetView,
        .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode        = resolveMode,
        .resolveImageView   = resolveView,
        .resolveImageLayout = resolveLayout,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue         = clearColor,
    };

    constexpr VkClearDepthStencilValue depthClear = {1.0f, 0u};

    const VkImageView depthTargetView = msaa ? m_depthOutputMsaa->view() : m_depthOutput->view();

    const VkRenderingAttachmentInfoKHR depthAttachment = {
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext       = nullptr,
        .imageView   = depthTargetView,
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,

        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,

        .loadOp     = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp    = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = {.depthStencil = depthClear},
    };

    const VkRenderingInfoKHR renderInfo = {
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .pNext                = nullptr,
        .flags                = 0,
        .renderArea           = {.offset = {0, 0}, .extent = {m_extent}},
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &colorAttachment,
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = nullptr
    };
    vkCmdBeginRendering(cmdBuffer, &renderInfo);

    const VkViewport viewport = {
        .x        = 0,
        .y        = 0,
        .width    = float(m_extent.width),
        .height   = float(m_extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = m_extent,
    };
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
}

void LightningPass::EndPass(const VkCommandBuffer cmdBuffer) const
{
    vkCmdEndRendering(cmdBuffer);
    TransitionForRead(cmdBuffer);
}

void LightningPass::TransitionForRender(const VkCommandBuffer cmdBuffer) const
{
    VkImageMemoryBarrier2 msaaBarrier = {.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                         .pNext               = nullptr,
                                         .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                         .srcAccessMask       = VK_ACCESS_2_NONE,
                                         .dstStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                         .dstAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                         .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED, // Discard old data
                                         .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .image               = m_colorOutputMsaa->image(),
                                         .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    VkImageMemoryBarrier2 depthMsaaBarrier = {.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                              .pNext               = nullptr,
                                              .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                              .srcAccessMask       = VK_ACCESS_2_NONE,
                                              .dstStageMask        = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
                                              .dstAccessMask       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                              .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED, // Discard old data
                                              .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                              .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                              .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                              .image               = m_depthOutputMsaa->image(),
                                              .subresourceRange    = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1}};


    VkImageMemoryBarrier2 resolveBarrier = {.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                            .pNext               = nullptr,
                                            .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                            .srcAccessMask       = VK_ACCESS_2_NONE,
                                            .dstStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                            .dstAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
                                            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                            .image               = m_colorOutput->image(),
                                            .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    VkImageMemoryBarrier2 depthBarrier = {.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                          .pNext               = nullptr,
                                          .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                                          .srcAccessMask       = VK_ACCESS_2_NONE,
                                          .dstStageMask        = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
                                          .dstAccessMask       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                          .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
                                          .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                                          .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                          .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                          .image = m_depthOutput->image(),
                                          .subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1}};


    VkImageMemoryBarrier2 barriers[] = {msaaBarrier, depthMsaaBarrier, resolveBarrier, depthBarrier};

    VkDependencyInfo depInfo = {.sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
                                .pNext                    = nullptr,
                                .dependencyFlags          = 0,
                                .memoryBarrierCount       = 0,
                                .pMemoryBarriers          = nullptr,
                                .bufferMemoryBarrierCount = 0,
                                .pBufferMemoryBarriers    = nullptr,
                                .imageMemoryBarrierCount  = 4,
                                .pImageMemoryBarriers     = barriers};

    vkCmdPipelineBarrier2(cmdBuffer, &depInfo);
}

void LightningPass::TransitionForRead(const VkCommandBuffer cmdBuffer) const
{
    const VkImageMemoryBarrier2 colorBarrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                                                .pNext = nullptr,
                                                .srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                                                .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                                                .dstStageMask  = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
                                                .dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
                                                .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                .newLayout           = VK_IMAGE_LAYOUT_GENERAL,
                                                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                .image            = m_colorOutput->image(),
                                                .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    const VkDependencyInfo depInfo = {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0,
        .memoryBarrierCount       = 0,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers    = nullptr,
        .imageMemoryBarrierCount  = 1,
        .pImageMemoryBarriers     = &colorBarrier,
    };

    vkCmdPipelineBarrier2(cmdBuffer, &depInfo);
}