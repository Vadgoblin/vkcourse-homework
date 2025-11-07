#pragma once
#include "BasePrimitive.h"

class Cylinder : public BasePrimitive {
public:
    Cylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int sectorCount,
                   int stackCount,
                   bool wireframe = false);

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

private:
    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    uint32_t         m_vertexCount    = 0;

    float baseRadius, topRadius, height;
    int sectorCount, stackCount;
};