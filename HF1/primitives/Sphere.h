#pragma once
#include "BasePrimitive.h"

class Sphere : public BasePrimitive {
public:
    Sphere(float radius, int sectorCount, int stackCount);
};