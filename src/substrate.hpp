#pragma once
#include "entities.hpp"

enum class SubstrateType {soil, sand, gravel};

enum class SimpleCollisionType {none, left, right, full};

class Substrate: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
public:
    static SimpleCollisionType checkFallingCollision(const Entity & movingEntity,
        const vector<Substrate> & substrate
    );

    SubstrateType type = SubstrateType::soil;

    Substrate(SubstrateType substrateType);
    void stopSubstrate();
    void update(Environment & environment, vector<Substrate> & substrate, size_t currentSandIdx);
};