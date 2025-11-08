#pragma once
#include "BasePrimitive.h"

class Cone : public BasePrimitive {
public:
    Cone(float baseRadius = 1.0f, float height = 1.0f, int sectorCount = 3, bool capBase = true, bool wireframe = false);
};