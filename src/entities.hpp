#include "utilities.hpp"
#include <time.h>
#include <vector>
#include <limits>
#include <iostream>

using std::vector;

const int screenWidth = 800;
const int screenHeight = 450;
const int startingWaterLevel = screenHeight * 0.8;
const float gravity = 0.1f;
const float minSandFall = 0.5f;
const float maxParticleRadius = 10.0f;
const float speedLimit = 1.0f;
const int saturationFromAlgae = 300;

class Entity{
public:
    bool active = true;
    Vector2 pos;
    Vector2 velocity;
    float radius;
};

struct EcosystemLite{
public:
    float waterSurfaceY;
    float waterLevel;
    vector<Entity> algaes;
};

class Ostracod: public Entity{
    constexpr static Vector2 sizeRange = VEC2(2.0f, 8.0f);
    constexpr static Vector2 visionRange = VEC2(180.0f, 200.0f);
    constexpr static Vector2 reactionTimeRange = VEC2(5.0f, 20.0f);
    constexpr static Vector2 speedRange = VEC2(0.4f, 0.8f);
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
    void decideToMove(EcosystemLite & ecosystem, bool underwater);
    void update(EcosystemLite & ecosystem);
    Entity* findFirstVisibleAlgae(vector<Entity> & algaes);
};
