#pragma once
#include "entities.hpp"

enum class SubstrateType {soil, sand, gravel};

enum class SimpleCollisionType {none, left, right, full};

class Substrate: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
    constexpr static ColorRange SOIL_COLOR_RANGE = {BLACK, BLACK};
    constexpr static ColorRange GRAVEL_COLOR_RANGE = {{70, 70, 70, 255}, {90, 90, 90, 255}};
    constexpr static ColorRange SAND_COLOR_RANGE = {{200, 180, 0, 255}, {253, 200, 0, 255}};
public:
    static SimpleCollisionType checkFallingCollision(const Entity & movingEntity,
        const vector<Substrate> & substrate
    );

    SubstrateType type = SubstrateType::soil;

    Substrate(SubstrateType substrateType);
    void stopSubstrate();
    void update(Environment & environment, vector<Substrate> & substrate, size_t currentSandIdx);
};