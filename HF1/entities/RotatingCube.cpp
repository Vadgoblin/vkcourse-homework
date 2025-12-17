#include "RotatingCube.h"
#include "../primitives/Cube.h"


RotatingCube::RotatingCube()
{
    m_objectGroup = new ObjectGroup();
}

void RotatingCube::draw(const VkCommandBuffer cmdBuffer, bool lightingPass, const glm::mat4& parentModel)
{
    m_objectGroup->draw(cmdBuffer,lightingPass, parentModel * getModelMatrix());
}

void RotatingCube::create(Context& context, LightningPass& lightningPass, ShadowPass& shadowPass)
{
    Cube* cube = new Cube(true);
    cube->create(context,lightningPass, shadowPass, "grassblock_FIX");
    cube->setScale(0.5f,0.5f,0.5f);
    m_objectGroup->addChild(cube);
}

void RotatingCube::destroy(const VkDevice device)
{
    m_objectGroup->destroyChildren(device);
}

void RotatingCube::tick()
{
    m_time += 1.0f;

    m_objectGroup->setRotation(m_time * m_speed,0.0f,0.0f);
}
