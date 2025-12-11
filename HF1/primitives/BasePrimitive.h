#pragma once

#include "IDrawable.h"
#include "ITransformable.h"

#include <vulkan/vulkan_core.h>

#include "buffer.h"
#include "glm/fwd.hpp"
#include "glm_config.h"
#include <vector>
// #include "../ModelPushConstant.h"

class Context;

class BasePrimitive  : public ITransformable, public IDrawable{
public:

    BasePrimitive();
    ~BasePrimitive() override = default;

    VkResult create(const Context& context);
    void     destroy(VkDevice device);
    void     draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f)) override;



protected:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline       m_pipeline       = VK_NULL_HANDLE;
    uint32_t         m_constantOffset = 0;

    const uint32_t* m_shaderVertData = nullptr;
    size_t          m_shaderVertSize = 0;
    const uint32_t* m_shaderFragData = nullptr;
    size_t          m_shaderFragSize = 0;

    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    BufferInfo       m_texCoordBuffer   = {};

    std::vector<float>        m_vertices;
    std::vector<float>        m_normals;
    std::vector<float>        m_texCoords;
    std::vector<unsigned int> m_indices;

    uint32_t         m_vertexCount;
};