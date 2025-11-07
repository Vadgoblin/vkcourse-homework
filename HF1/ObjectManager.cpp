#include "ObjectManager.h"

#include "primitives/BasePrimitive.h"
#include "primitives/Cube.h"
#include "primitives/Cylinder.h"
#include "primitives/Grid.h"

#include <memory>

namespace ObjectManager {
std::vector<BasePrimitive*> objects;

void SetupAll(const Context& context, const Swapchain& swapchain, size_t pushConstansStart)
{
    Cube* cube = new Cube(true);
    cube->setPosition(0.0f, 0.45f,0.0f);
    cube->setScale(1.0f,0.9f,1.0f);
    cube->setRotation(15.0f,0.0f,0.0f);
    cube->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(cube);

    Cylinder* cylinder = new Cylinder(0.2, 0.2, 1, 25, 2, true);
    cylinder->setPosition(0.0f, 1.0f,0.0f);
    cylinder->Create(context, swapchain.format(), pushConstansStart);
    objects.push_back(cylinder);

    Grid* grid = new Grid(2,true);
    grid->Create(context, swapchain.format(), pushConstansStart);
    grid->setRotation(90.0f, 0.0f, 0.0f);
    grid->setScale(8.0f, 8.0f, 0.0f);
    objects.push_back(grid);
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