#include "utilities.hpp"
#include "environment.hpp"
#include <time.h>
#include <vector>
#include <limits>
#include <iostream>

using std::vector;

class Entity{
public:
    bool active = true;
    Vector2 pos = VEC2(0.0f, 0.0f);;
    Vector2 velocity = VEC2(0.0f, 0.0f);;
    float radius = 1.0f;
};

class Algae: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
public:
    Algae();
    void update(float waterSurfaceY);
};

class Sand: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
public:
    Sand();
    void settle();
    void update(Environment & ecosystem, vector<Sand> & sand, size_t currentSandIdx);
};

class Ostracod: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 8.0f);
    constexpr static Vector2 VISION_RANGE = VEC2(180.0f, 200.0f);
    constexpr static Vector2 REACTION_TIME_RANGE = VEC2(5.0f, 20.0f);
    constexpr static Vector2 SPEED_RANGE = VEC2(0.4f, 0.8f);
public:
    const static int baseSaturation = 600;
    const static int maxSaturation = 6000;
    const static int basicEnergyCost = 1;
    constexpr static float outOfWaterSpeedPenality = 0.9f;
    constexpr static float afterDeathSpeedPenalityPrecision = 0.1f;
    constexpr static float afterDeathSpeedPenality = 0.1f;

    bool alive = true;
    int saturation = 0.0f;
    float vision = 0.0f;
    float reactionTime = 0.0f;
    float reactionCooldown = 0.0f;
    float speed = 0.0f;

    Ostracod();
    Algae* findFirstVisibleAlgae(vector<Algae> & algaes);
    void decideToMove(Environment & ecosystem, vector<Algae> & algaes, bool underwater);
    void update(Environment & ecosystem, vector<Algae> & algaes);
};
