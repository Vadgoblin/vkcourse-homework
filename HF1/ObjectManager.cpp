#include "ObjectManager.h"

#include "primitives/BasePrimitive.h"
#include "primitives/Cube.h"
#include "primitives/Cylinder.h"
#include "primitives/Grid.h"
#include "primitives/Sphere.h"

namespace ObjectManager {
std::vector<BasePrimitive*> objects;

void SetupAll(const Context& context, const Swapchain& swapchain, size_t pushConstansStart)
{
    Grid* grid = new Grid(1,1,3,2,false);
    grid->Create(context, swapchain.format(), pushConstansStart);
    grid->setScale(12.0f, 1.0f, 12.0f);
    objects.push_back(grid);

    Cube* cube = new Cube(1.0f, true);
    cube->setPosition(0.0f, 0.45f,0.0f);
    cube->setScale(1.0f,0.9f,1.0f);
    cube->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(cube);

    Cylinder* cylinder = new Cylinder(0.05, 0.05, 1, 25, 2, true);
    cylinder->setPosition(0.0f, 0.5f,0.0f);
    cylinder->setRotation(90.0f,0.0f,0.0f);
    cylinder->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(cylinder);

    Cube* cube2 = new Cube(1.0f, true);
    cube2->setPosition(0.0f, 0.95f,0.0f);
    cube2->setScale(1.0f,0.1f,1.0f);
    cube2->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(cube2);

    Sphere* sphere = new Sphere(0.4f,25,25,true);
    sphere->setPosition(0.0f, 1.4f,0.0f);
    sphere->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(sphere);
}

void DrawAll(VkCommandBuffer cmd)
{
    for (BasePrimitive* object : objects) {
        object->Draw(cmd);
    }
}

void DestroyAll(VkDevice device) {
    for (BasePrimitive* object : objects) {
        object->Destroy(device);
    }
}


}