#pragma once
#include "entities.hpp"

class Algae: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
public:
    Algae();
    void update(float waterSurfaceY);
};