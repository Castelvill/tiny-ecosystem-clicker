#pragma once
#include "entities.hpp"

//TODO: Remove the types or update floating_algae
enum class AlgaeType : int {
    floating_algae, //green water algae (ex. Phytoplankton) - grows only on the water surface
    glass_algae //periphyton (ex.: Green Spot Algae, Green Dust Algae) - grows on submerged surfaces
};

class Algae: public Entity{
    constexpr static Vector2 SIZE_RANGE = VEC2(2.0f, 3.0f);
    constexpr static Vector2 LARGE_SIZE_RANGE = VEC2(4.0f, 6.0f);
    constexpr static ColorRange NORMAL_COLOR_RANGE = {{0, 208, 28, 255}, {20, 248, 68, 255}};
    constexpr static ColorRange DORMANT_COLOR_RANGE = {DARKGREEN, {20, 137, 64, 255}};
    constexpr static IntRange MAX_LIFESPAN_RANGE = {600, 1200};
    constexpr static IntRange DORMANT_STATE_DURATION_RANGE = {600, 1200};
    constexpr static float AGING_SPEED_IN_DORMANT_STATE = 0.01;
    constexpr static float TIME_UNTIL_GOING_DORMANT = 500;

    //glass_algae
    constexpr static int TIME_TO_ATTACH = 100;

    void waitForReproduction(float waterLevel, const vector<Algae> & algaes,
        size_t itsIdx
    );
    void updateLifespan();
    void updateOutsideTheWater();
    void updateInsideTheWater(const Environment & environment, const vector<Algae> & algaes,
        size_t itsIdx
    );
public:
    constexpr static IntRange REPRODUCTION_COOLDOWN = {60 * 5, 60 * 10};
    constexpr static float chanceToTurnIntoSubstrate = 0.001;

    AlgaeType type = AlgaeType::floating_algae;
    int timeToReproduce = 0;
    bool alive = true;
    float lifespan = 0.0f;
    int timeUntilGoingDormant = TIME_UNTIL_GOING_DORMANT;
    int dormantTimer = 0;

    //glass_algae
    int timeToAttach = 0;
    bool attached = false;

    bool isPlayerMade = true;
    size_t sectorIdx = 0;

    void resetReproduction();
    Algae(AlgaeType newType);
    void reproduce(Vector2 parentPosition, const Environment & environment, bool enlarge);
    void update(const Environment & environment, const vector<Algae> & algaes, size_t itsIdx);
};