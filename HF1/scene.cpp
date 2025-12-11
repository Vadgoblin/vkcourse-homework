#include "scene.h"

#include "containers/ObjectGroup.h"
#include "entities/OrbitingHelicopter.h"
#include "entities/PistonWithBouncingBall.h"
#include "entities/RotatingCube.h"
#include "entities/RotatingTetrahedron.h"
#include "primitives/BasePrimitive.h"
#include "primitives/Cone.h"
#include "primitives/Cube.h"
#include "primitives/Cylinder.h"
#include "primitives/Grid.h"
#include "primitives/Sphere.h"

namespace ObjectManager {
std::vector<BasePrimitive*> primitives;
std::vector<ObjectGroup*> objectGroups;
std::vector<BaseEntity*> entities;

void setup(const Context& context)
{
    Grid* grid = new Grid(1,1,3,2,false);
    grid->create(context);
    grid->setScale(12.0f, 1.0f, 12.0f);
    primitives.push_back(grid);

    RotatingCube* rotatingCube = new RotatingCube();
    rotatingCube->create(context);
    rotatingCube->setPosition(3.0f, 2.0f, 0.0f);
    entities.push_back(rotatingCube);

    RotatingTetrahedron* rotating_tetrahedron = new RotatingTetrahedron();
    rotating_tetrahedron->create(context);
    rotating_tetrahedron->setPosition(-4.0f, 1.2f, 0.0f);
    entities.push_back(rotating_tetrahedron);

    PistonWithBouncingBall* piston_with_bouncing_ball = new PistonWithBouncingBall();
    piston_with_bouncing_ball->create(context);
    entities.push_back(piston_with_bouncing_ball);

    OrbitingHelicopter* orbiting_helicopter = new OrbitingHelicopter();
    orbiting_helicopter->create(context);
    orbiting_helicopter->setPosition(0.0f, 5.0f, 0.0f);
    entities.push_back(orbiting_helicopter);
}

void draw(const VkCommandBuffer cmd)
{
    for (BaseEntity* object : entities) {
        object->tick();
        object->draw(cmd);
    }
    for (ObjectGroup* object : objectGroups) {
        object->draw(cmd);
    }
    for (BasePrimitive* object : primitives) {
        object->draw(cmd);
    }
}

void destroy(const VkDevice device) {
    for (BaseEntity* object : entities) {
        object->destroy(device);
        delete object;
    }
    entities.clear();

    for (ObjectGroup* object : objectGroups) {
        object->destroyChildren(device);
        delete object;
    }
    objectGroups.clear();

    for (BasePrimitive* object : primitives) {
        object->destroy(device);
        delete object;
    }
    primitives.clear();
}


}