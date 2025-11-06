#pragma once
#include "BasePrimitive.h"

class Cube : public BasePrimitive {
    public:
    Cube(bool wireframe = false);

    VkResult Create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart);
    void     Destroy(const VkDevice device);
    void     Draw(const VkCommandBuffer cmdBuffer);

    private:
    uint32_t         m_vertexCount    = 0;
};
