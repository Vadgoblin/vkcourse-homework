#include "SpinningCirnoPrism.h"

#include "../primitives/CirnoPrism.h"
#include "../primitives/Cone.h"

SpinningCirnoPrism::SpinningCirnoPrism()
{
    m_objectGroup = new ObjectGroup();
}

SpinningCirnoPrism::~SpinningCirnoPrism()
{
    delete m_objectGroup;
}

void SpinningCirnoPrism::draw(const VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    m_objectGroup->draw(cmdBuffer, parentModel * getModelMatrix());
}

void SpinningCirnoPrism::create(Context& context)
{
    CirnoPrism* cirnoPrism = new CirnoPrism();
    cirnoPrism->create(context, "Cirno Prism");
    m_objectGroup->addChild(cirnoPrism);
}

void SpinningCirnoPrism::destroy(const VkDevice device)
{
    m_objectGroup->destroyChildren(device);
}

void SpinningCirnoPrism::tick()
{
    m_time += 4.0f;

    m_objectGroup->setRotation(0.0f,m_time * m_speed,0.0f);
}
