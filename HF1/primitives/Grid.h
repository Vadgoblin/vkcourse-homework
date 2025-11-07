#pragma once
#include "BasePrimitive.h"

class Grid : public BasePrimitive {
public:
    Grid(uint subdivisions, bool wireframe);

    VkResult Create(const Context& context,
                    const VkFormat colorFormat,
                    const uint32_t pushConstantStart);

    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

private:
    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    uint32_t         m_vertexCount    = 0;
    uint             m_subdivisions;
};