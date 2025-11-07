#pragma once
#include "BasePrimitive.h"

class Cylinder : public BasePrimitive {
public:
    Cylinder(float baseRadius,
                   float topRadius,
                   float height,
                   int sectorCount,
                   int stackCount,
                   bool wireframe = false);
};