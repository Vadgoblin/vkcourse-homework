#pragma once
#include "../primitives/IDrawable.h"
#include "../primitives/ITransformable.h"

#include <context.h>

class BaseEntity : public IDrawable, public ITransformable {
public:
    BaseEntity() = default;
    virtual ~BaseEntity() = default;
    virtual void create(const Context& context, VkFormat colorFormat, uint32_t pushConstantStart, VkPipeline pipeline ) = 0;
    virtual void destroy(VkDevice device) = 0;
    virtual void tick() = 0;
};