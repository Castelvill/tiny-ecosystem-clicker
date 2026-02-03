#pragma once

#include "environment.hpp"
#include <time.h>
#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <format>

using std::vector;

class Entity{
public:
    bool active = true;
    Vector2 pos = VEC2(0.0f, 0.0f);;
    Vector2 velocity = VEC2(0.0f, 0.0f);;
    float radius = 1.0f;
    Color color;
    
    bool detectCollisionWithAquarium(const Vector2 nextPosition, const Environment & environment,
        bool ignoreVelocity = false, bool infinitWalls = true
    );
    bool checkIfUnderwater(Environment & environment);
    void applyGravityAndBuoyancy(Environment & environment, bool isUnderwater, float mass);

    void saveToFile(std::ofstream & file) const;
    void loadFromFile(std::ifstream & file);
};