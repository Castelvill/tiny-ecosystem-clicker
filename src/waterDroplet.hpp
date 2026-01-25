#pragma once
#include "entities.hpp"

class WaterDroplet: public Entity {
    constexpr static Vector2 SIZE_RANGE = VEC2(4.0f, 6.0f);
public:
    WaterDroplet();
    void update(Environment & environment);
};