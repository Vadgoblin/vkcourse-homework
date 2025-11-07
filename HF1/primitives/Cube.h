#pragma once
#include "BasePrimitive.h"

class Cube : public BasePrimitive {
public:
    Cube(const float size = 1.0f, const bool wireframe = false);

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

private:
    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    uint32_t         m_vertexCount    = 0;

    float m_size;
};
