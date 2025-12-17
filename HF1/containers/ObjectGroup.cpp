#include "ObjectGroup.h"

void ObjectGroup::addChild(IDrawable* child)
{
    m_children.push_back(child);
}

void ObjectGroup::draw(const VkCommandBuffer cmdBuffer,bool lightningPass, const glm::mat4& parentModel)
{
    glm::mat4 finalModel = parentModel * getModelMatrix();

    for (auto& child : m_children)
        child->draw(cmdBuffer, lightningPass, finalModel);
}
void ObjectGroup::destroyChildren(const VkDevice device)
{
    for (const auto& child : m_children) {
        if (BasePrimitive* d = dynamic_cast<BasePrimitive*>(child)) {
            d->destroy(device);
        } else if (ObjectGroup* d = dynamic_cast<ObjectGroup*>(child)) {
            d->destroyChildren(device);
        }
        delete child;
    }
    m_children.clear();
}

