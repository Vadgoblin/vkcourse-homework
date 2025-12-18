#pragma once
#include "../containers/ObjectGroup.h"
#include "../entities/BaseEntity.h"
#include "../primitives/BasePrimitive.h"

#include <context.h>

class ObjectManager {
public:
    explicit ObjectManager(Context& context, LightningPass& lightningPass, ShadowPass& shadowPass);

    void Draw(VkCommandBuffer cmd, bool lightPass);
    void Tick();
    void Destroy(VkDevice device);

private:
    std::vector<BasePrimitive*> m_primitives   = std::vector<BasePrimitive*>();
    std::vector<ObjectGroup*>   m_objectGroups = std::vector<ObjectGroup*>();
    std::vector<BaseEntity*>    m_entities     = std::vector<BaseEntity*>();
};
