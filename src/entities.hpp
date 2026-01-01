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
    constexpr static Vector2 SPEED_RANGE = VEC2(0.01f, 0.02f);
public:
    const static int BASE_SATURATION = 6000;
    const static int MAX_SATURATION = 60000;
    const static int STARVING_TRIGGER = MAX_SATURATION * 0.5;
    const static int BASIC_ENERGY_COST = 1;
    const static int BASIC_EATING_COOLDOWN = 90;
    constexpr static float OUTSIDE_WATER_SPEED_PENALITY = 0.9f;
    constexpr static float AFTER_DEATH_SPEED_PENALITY_PRECISION = 0.1f;
    constexpr static float AFTER_DEATH_SPEED_PENALITY = 0.1f;

    bool alive = true;
    int saturation = 0;
    int reactionTime = 0;
    int reactionCooldown = 0;
    int eatingCooldown = 0;
    float vision = 0.0f;
    float speed = 0.0f;

    Ostracod();
    Algae* findNearestAlgae(vector<Algae> & algaes);
    void decideToMove(Environment & ecosystem, vector<Algae> & algaes, bool underwater);
    void update(Environment & ecosystem, vector<Algae> & algaes, vector<Sand> & sand,
        size_t & aliveOstrecods
    );
};
