#pragma once
#include "BasePrimitive.h"

class Grid : public BasePrimitive {
public:
    Grid(float width = 1,float depth = 1, int rows = 1, int cols = 1, bool wireframe = false);

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
    float            m_width;
    float m_depth;
    int m_rows;
    int m_cols;
};