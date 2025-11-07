#pragma once
#include "BasePrimitive.h"

class Sphere : public BasePrimitive {
public:
    Sphere(float radius, int sectorCount, int stackCount, bool wireframe);

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

private:
    BufferInfo       m_vertexBuffer   = {};
    BufferInfo       m_indexBuffer    = {};
    uint32_t         m_vertexCount    = 0;

    float m_radius;
    int m_sectorCount;
    int m_stackCount;
};