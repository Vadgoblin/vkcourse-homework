#include "ObjectGroup.h"

void ObjectGroup::addChild(IDrawable* child)
{
    m_children.push_back(child);
}

void ObjectGroup::draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    glm::mat4 finalModel = parentModel * getModelMatrix();

    for (auto& child : m_children)
        child->draw(cmdBuffer, finalModel);
}

