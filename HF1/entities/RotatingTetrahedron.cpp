#include "RotatingTetrahedron.h"

#include "../primitives/Cone.h"

RotatingTetrahedron::RotatingTetrahedron()
{
    m_objectGroup = new ObjectGroup();
}

RotatingTetrahedron::~RotatingTetrahedron()
{
    delete m_objectGroup;
}

void RotatingTetrahedron::draw(const VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    m_objectGroup->draw(cmdBuffer, parentModel * getModelMatrix());
}

void RotatingTetrahedron::create(const Context& context, const VkFormat colorFormat, const uint32_t pushConstantStart, VkPipeline pipeline )
{
    Cone* cone = new Cone();
    cone->create(context, colorFormat, pushConstantStart, pipeline);
    m_objectGroup->addChild(cone);
}

void RotatingTetrahedron::destroy(const VkDevice device)
{
    m_objectGroup->destroyChildren(device);
}

void RotatingTetrahedron::tick()
{
    m_time += 1.0f;

    m_objectGroup->setRotation(0.0f,m_time * m_speed,0.0f);
}
