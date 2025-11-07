#pragma once
#include "BasePrimitive.h"

class Grid : public BasePrimitive {
public:
    Grid();

    VkResult Create(const Context& context,
                    const VkFormat colorFormat,
                    const uint32_t pushConstantStart,
                    float          width,
                    float          height,
                    uint32_t       count);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

private:
    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    uint32_t         m_vertexCount    = 0;
};