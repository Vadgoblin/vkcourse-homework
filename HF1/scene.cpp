#include "scene.h"

#include "containers/ObjectGroup.h"
#include "entities/RotatingCube.h"
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

void setup(const Context& context, const Swapchain& swapchain, size_t pushConstansStart)
{
    ObjectGroup* group1 = new ObjectGroup();
    ObjectGroup* group2 = new ObjectGroup();
    group1->addChild(group2);

    Grid* grid = new Grid(1,1,3,2,false);
    grid->create(context, swapchain.format(), pushConstansStart);
    grid->setScale(12.0f, 1.0f, 12.0f);
    // objects.push_back(grid);
    group1->addChild(grid);

    Cube* cube = new Cube(1.0f, true);
    cube->setPosition(0.0f, 0.45f,0.0f);
    cube->setScale(1.0f,0.9f,1.0f);
    cube->create(context, swapchain.format(), pushConstansStart);
    // objects.push_back(cube);
    group2->addChild(cube);

    Cylinder* cylinder = new Cylinder(0.05, 0.05, 1, 25, 2, true);
    cylinder->setPosition(0.0f, 0.5f,0.0f);
    cylinder->setRotation(90.0f,0.0f,0.0f);
    cylinder->create(context, swapchain.format(), pushConstansStart);
    // objects.push_back(cylinder);
    group2->addChild(cylinder);

    Cube* cube2 = new Cube(1.0f, true);
    cube2->setPosition(0.0f, 0.95f,0.0f);
    cube2->setScale(1.0f,0.1f,1.0f);
    cube2->create(context, swapchain.format(), pushConstansStart);
    // objects.push_back(cube2);
    group2->addChild(cube2);

    Sphere* sphere = new Sphere(0.4f,25,25,true);
    sphere->setPosition(0.0f, 1.4f,0.0f);
    sphere->create(context, swapchain.format(), pushConstansStart);
    // objects.push_back(sphere);
    group2->addChild(sphere);


    RotatingCube* rotatingCube = new RotatingCube();
    rotatingCube->create(context, swapchain.format(), pushConstansStart);
    rotatingCube->setPosition(3.0f, 2.0f, 0.0f);
    entities.push_back(rotatingCube);
    // group1->addChild(rotatingCube);

    Cone* cone = new Cone(1.0f,1.0f,3, true, false);
    cone->create(context, swapchain.format(), pushConstansStart);
    cone->setPosition(-4.0f, 1.2f, 0.0f);
    primitives.push_back(cone);


    // group2->setRotation(0.0f, 0.0f,-14.0f);
    // group1->setRotation(0.0f, 0.0f,14.0f);
    // group1->setPosition(0.0f, -4.0f,0.0f);

    objectGroups.push_back(group1);
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