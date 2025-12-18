#pragma once
#include "../render_passes/ShadowPass.h"
#include "../render_passes/LightningPass.h"
#include "../primitives/IDrawable.h"
#include "../primitives/ITransformable.h"


class BaseEntity : public IDrawable, public ITransformable {
public:
    BaseEntity() = default;
    virtual ~BaseEntity() = default;
    virtual void create(Context& context, LightningPass& lightningPass, ShadowPass& shadowPass) = 0;
    virtual void destroy(VkDevice device) = 0;
    virtual void tick() = 0;
};