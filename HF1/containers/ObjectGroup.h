#pragma once
#include "../primitives/BasePrimitive.h"
#include "../primitives/ITransformable.h"

#include <vector>
#include <vulkan/vulkan_core.h>

class ObjectGroup : public ITransformable {
public:
    void     addChild(BasePrimitive *);
    void     draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f));

protected:
    std::vector<BasePrimitive*> m_children;
};