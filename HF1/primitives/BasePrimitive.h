#pragma once

#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "glm/fwd.hpp"
#include "glm_config.h"
#include <vector>

namespace {
#include "triangle_in.frag_include.h"
#include "triangle_in.vert_include.h"

}

class Context;

class BasePrimitive {
public:


    BasePrimitive(bool wireframe = false);
    virtual ~BasePrimitive() = default;

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

    void setScale(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setRotation(float rx, float ry, float rz);

protected:
    struct ModelPushConstant {
        glm::mat4 model;
    };

    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline       m_pipeline       = VK_NULL_HANDLE;
    uint32_t         m_constantOffset = 0;

    const uint32_t*  m_shaderVertData   = SPV_triangle_in_vert;
    size_t           m_shaderVertSize   = sizeof(SPV_triangle_in_vert);
    const uint32_t*  m_shaderFragData   = SPV_triangle_in_frag;
    size_t           m_shaderFragSize   = sizeof(SPV_triangle_in_frag);

    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};

    std::vector<float>        m_vertices;
    std::vector<float>        m_normals;
    std::vector<float>        m_texCoords;
    std::vector<unsigned int> m_indices;

    uint32_t         m_vertexCount;


    glm::mat4        m_scale          = glm::mat4(1.0f);
    glm::mat4        m_position       = glm::mat4(1.0f);
    glm::mat4        m_rotation       = glm::mat4(1.0f);

    bool wireframe;
};