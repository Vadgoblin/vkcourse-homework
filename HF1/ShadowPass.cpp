#include "ShadowPass.h"
#include "context.h"
#include "primitives/BasePrimitive.h"
#include "wrappers.h"

namespace {
    #include "shaders/shadow_map.frag_include.h"
    #include "shaders/shadow_map.vert_include.h"
}

VkPipelineLayout CreatePipelineLayout(const VkDevice                            device,
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

VkPipeline BuildPipeline(const VkDevice device, const VkPipelineLayout pipelineLayout, const VkFormat depthFormat)
{
    VkShaderModule shaderVertex   = CreateShaderModule(device, SPV_shadow_map_vert, sizeof(SPV_shadow_map_vert));
    VkShaderModule shaderFragment = CreateShaderModule(device, SPV_shadow_map_frag, sizeof(SPV_shadow_map_frag));

    // shader stages
    VkPipelineShaderStageCreateInfo shaders[] = {
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

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {
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
    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_NONE, // VK_CULL_MODE_FRONT_BIT,
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable         = VK_TRUE,
        .depthBiasConstantFactor = 0.5f,
        .depthBiasClamp          = 0.0f,  // Disabled
        .depthBiasSlopeFactor    = 1.75f, // Disabled
        .lineWidth               = 1.0f,
    };

    // multisample
    VkPipelineMultisampleStateCreateInfo multisampleInfo = {
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
    VkStencilOpState emptyStencilOp = {}; // Removed memeber init for space

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = VK_TRUE, // Depth test is a must!
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = emptyStencilOp,
        .back                  = emptyStencilOp,
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };

    VkPipelineColorBlendAttachmentState blendAttachment = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp       = VK_LOGIC_OP_CLEAR,
        .attachmentCount = 1,
        .pAttachments    = &blendAttachment,
        .blendConstants  = {1.0f, 1.0f, 1.0f, 1.0f},
    };

    const VkPipelineRenderingCreateInfo renderingInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = 0,
        .pColorAttachmentFormats = nullptr,
        .depthAttachmentFormat   = depthFormat,
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
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
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
    VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,&pipeline);

    vkDestroyShaderModule(device, shaderVertex, nullptr);
    vkDestroyShaderModule(device, shaderFragment, nullptr);

    assert(result == VK_SUCCESS);

    return pipeline;
}

ShadowPass::ShadowPass(Context& context, LightManager& lightManager, VkFormat depthFormat, VkExtent2D extent)
    : m_depthFormat(depthFormat)
    , m_lightManager(lightManager)
    , m_extent(extent)

{
    VkPhysicalDevice phyDevice = context.physicalDevice();
    VkDevice         device    = context.device();

    for (int i = 0; i < LightManager::NumberOfLights(); i++) {
        Texture* t = Texture::Create2D(phyDevice, device, m_depthFormat, m_extent,
                                      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        assert(t->IsValid());
        m_shadowDepths.push_back(t);
    }

    uint32_t pushConstantSize = sizeof(LightInfoPushConstant) + sizeof(BasePrimitive::ModelPushConstant);
    m_modelPushConstantOffset = sizeof(LightInfoPushConstant);
    m_pipelineLayout = CreatePipelineLayout(device, {BasePrimitive::CreateVertexDataDescSetLayout(context)}, pushConstantSize);
    m_pipeline = BuildPipeline(device,m_pipelineLayout,depthFormat);

    VkDescriptorSetLayoutBinding shadowMapDescSetLayoutBinding {
        .binding            = 0,
        .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount    = LightManager::NumberOfLights(),
        .stageFlags         = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr,
    };
    m_shadowMapDescSetLayout = context.descriptorPool().CreateLayout({shadowMapDescSetLayoutBinding});
    m_shadowMapDescSet = context.descriptorPool().CreateSet(m_shadowMapDescSetLayout);

    std::vector<VkDescriptorImageInfo> shadowImageInfos(LightManager::NumberOfLights());
    for (int i = 0; i < LightManager::NumberOfLights(); i++) {
        shadowImageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        shadowImageInfos[i].imageView = m_shadowDepths[i]->view();
        shadowImageInfos[i].sampler = m_shadowDepths[i]->sampler();
    }

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = {m_shadowMapDescSet};
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    descriptorWrite.descriptorCount = static_cast<uint32_t>(shadowImageInfos.size());
    descriptorWrite.pImageInfo = shadowImageInfos.data();

    vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void ShadowPass::Destroy(VkDevice device) const
{
    for (auto element : m_shadowDepths) {
        element->Destroy(device);
        delete element;
    }

    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
    vkDestroyPipeline(device, m_pipeline, nullptr);
}

void ShadowPass::BindDescriptorSets(VkCommandBuffer cmdBuffer, VkPipelineLayout pipelineLayout)
{
    vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 2, 1, &m_shadowMapDescSet, 0,nullptr);
}

void ShadowPass::TransitionForRender(const VkCommandBuffer cmdBuffer)
{
    std::vector<VkImageMemoryBarrier2> imageMemoryBarriers;
    for (auto element : m_shadowDepths) {
        const VkImageMemoryBarrier2 renderStartBarrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_2_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .dstAccessMask       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = element->image(),
            .subresourceRange =
                {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            }
        };
        imageMemoryBarriers.push_back(renderStartBarrier);
    }

    const VkDependencyInfo startDependency = {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0,
        .memoryBarrierCount       = 0,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers    = nullptr,
        .imageMemoryBarrierCount  = static_cast<uint32_t>(imageMemoryBarriers.size()),
        .pImageMemoryBarriers     = imageMemoryBarriers.data(),
    };
    vkCmdPipelineBarrier2(cmdBuffer, &startDependency);
}

void ShadowPass::TransitionForRead(const VkCommandBuffer cmdBuffer)
{
    std::vector<VkImageMemoryBarrier2> imageMemoryBarriers;
    for (auto element : m_shadowDepths) {
        const VkImageMemoryBarrier2 renderEndBarrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
            .srcAccessMask       = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
            .dstAccessMask       = VK_ACCESS_2_SHADER_READ_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = element->image(),
            .subresourceRange =
                {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            }
        };
        imageMemoryBarriers.push_back(renderEndBarrier);
    }

    const VkDependencyInfo startDependency = {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0,
        .memoryBarrierCount       = 0,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers    = nullptr,
        .imageMemoryBarrierCount  = static_cast<uint32_t>(imageMemoryBarriers.size()),
        .pImageMemoryBarriers     = imageMemoryBarriers.data(),
    };
    vkCmdPipelineBarrier2(cmdBuffer, &startDependency);
}

void ShadowPass::BeginNthPass(VkCommandBuffer cmdBuffer, const uint8_t n)
{
    const VkClearDepthStencilValue     depthClear      = {1.0f, 0u};
    const VkRenderingAttachmentInfoKHR depthAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext              = nullptr,
        .imageView          = m_shadowDepths[n]->view(),
        .imageLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue         = {.depthStencil = depthClear},
    };
    const VkRenderingInfoKHR renderInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .renderArea =
            {
            .offset = {0, 0},
            .extent = m_extent,
        },
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 0,
        .pColorAttachments    = nullptr,
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = nullptr,
    };
    vkCmdBeginRendering(cmdBuffer, &renderInfo);

    const VkViewport viewport = {
        .x        = 0,
        .y        = 0,
        .width    = static_cast<float>(m_extent.width),
        .height   = static_cast<float>(m_extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = m_extent,
    };
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    const LightInfoPushConstant lightInfo = {
        m_lightManager.light(n).projection,
        m_lightManager.light(n).view,
    };
    vkCmdPushConstants(cmdBuffer, m_pipelineLayout, VK_SHADER_STAGE_ALL,0, sizeof(lightInfo), &lightInfo);
}


void ShadowPass::EndNthPass(VkCommandBuffer cmdBuffer)
{
    vkCmdEndRendering(cmdBuffer);
}
