#pragma once
#include "../primitives/BasePrimitive.h"
#include "../primitives/ITransformable.h"

#include <vector>
#include <vulkan/vulkan_core.h>

class ObjectGroup : public ITransformable, public IDrawable{
public:
    void     addChild(IDrawable *);
    void     draw(VkCommandBuffer cmdBuffer, bool lightningPass, const glm::mat4& parentModel = glm::mat4(1.0f)) override;
    void     destroyChildren(VkDevice device);

protected:
    std::vector<IDrawable*> m_children;
};