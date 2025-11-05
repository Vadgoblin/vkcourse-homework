#pragma once

#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "glm/fwd.hpp"
#include "glm_config.h"

class Context;

class SimpleCube {
public:
    struct ModelPushConstant {
        glm::mat4 model;
    };

    SimpleCube();

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

    void setScale(float x, float y, float z);
    void setPosition(float x, float y, float z);

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline       m_pipeline       = VK_NULL_HANDLE;
    uint32_t         m_constantOffset = 0;
    BufferInfo       m_buffer         = {};
    uint32_t         m_vertexCount    = 0;
    glm::mat4        m_scale    = glm::mat4(1.0f);
    glm::mat4        m_position       = glm::mat4(1.0f);
    glm::mat4        m_rotation       = glm::mat4(1.0f);
};
