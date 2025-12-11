#pragma once
#include "BasePrimitive.h"

class Grid : public BasePrimitive {
public:
    Grid(float width = 1,float depth = 1, int rows = 1, int cols = 1);
};