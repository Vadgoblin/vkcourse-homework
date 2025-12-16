#include "ObjectManager.h"

#include "entities/OrbitingHelicopter.h"
#include "entities/PistonWithBouncingBall.h"
#include "entities/RotatingCube.h"
#include "entities/SpinningCirnoPrism.h"
#include "primitives/Grid.h"

ObjectManager::ObjectManager(Context& context)
{
    Grid* grid = new Grid(1, 1, 1, 1);
    // grid->create(context, "grass2");
    grid->create(context, "white");
    grid->setScale(12.0f, 1.0f, 12.0f);
    m_primitives.push_back(grid);

    RotatingCube* rotatingCube = new RotatingCube();
    rotatingCube->create(context);
    rotatingCube->setPosition(3.0f, 2.0f, 0.0f);
    m_entities.push_back(rotatingCube);

    SpinningCirnoPrism* rotating_tetrahedron = new SpinningCirnoPrism();
    rotating_tetrahedron->create(context);
    rotating_tetrahedron->setPosition(-4.0f, 1.2f, 0.0f);
    m_entities.push_back(rotating_tetrahedron);

    PistonWithBouncingBall* piston_with_bouncing_ball = new PistonWithBouncingBall();
    piston_with_bouncing_ball->create(context);
    m_entities.push_back(piston_with_bouncing_ball);

    OrbitingHelicopter* orbiting_helicopter = new OrbitingHelicopter();
    orbiting_helicopter->create(context);
    orbiting_helicopter->setPosition(0.0f, 5.0f, 0.0f);
    m_entities.push_back(orbiting_helicopter);
}
void ObjectManager::Draw(VkCommandBuffer cmd)
{
    for (BaseEntity* object : m_entities) {
        object->tick();
        object->draw(cmd);
    }
    for (ObjectGroup* object : m_objectGroups) {
        object->draw(cmd);
    }
    for (BasePrimitive* object : m_primitives) {
        object->draw(cmd);
    }
}
void ObjectManager::Destroy(const VkDevice device)
{
    for (BaseEntity* object : m_entities) {
        object->destroy(device);
        delete object;
    }
    m_entities.clear();

    for (ObjectGroup* object : m_objectGroups) {
        object->destroyChildren(device);
        delete object;
    }
    m_objectGroups.clear();

    for (BasePrimitive* object : m_primitives) {
        object->destroy(device);
        delete object;
    }
    m_primitives.clear();
}