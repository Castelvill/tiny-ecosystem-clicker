#pragma once
#include "entities.hpp"
#include "algae.hpp"
#include "substrate.hpp"

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

    //Behavior flags
    bool searchForFood = false;
    bool goInRandomDirection = false;
    bool goToWaterSurface = false;

    Ostracod();
    Algae* findNearestAlgae(vector<Algae> & algaes);
    void thinkAboutTheNextMove(const Environment & environment, vector<Algae> & algaes,
        Entity*& visibleAlgae
    );
    void thinkAndMove(Environment & environment, vector<Algae> & algaes, bool isUnderwater);
    //Movement when alive or dead
    void move(Environment & environment, vector<Algae> & algaes, bool isUnderwater);
    bool detectCollisionsWithSubstrate(vector<Substrate> & substrate, const Vector2 nextPosition);
    //Eat one algae
    void eatAlgae(vector<Algae> & algaes);
    void update(Environment &environment, vector<Algae> &algaes, vector<Substrate> &substrate);
    void starveAndDie();
};