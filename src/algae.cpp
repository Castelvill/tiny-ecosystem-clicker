#include "algae.hpp"

Algae::Algae(AlgaeType newType){
    type = newType;
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(SIZE_RANGE);
    color = randomBetween(DORMANT_COLOR_RANGE);
    timeUntilGoingDormant = TIME_UNTIL_GOING_DORMANT;
    dormantTimer = randomBetween(DORMANT_STATE_DURATION_RANGE);
    alive = true;
    lifespan = randomBetween(MAX_LIFESPAN_RANGE);
    resetReproduction();
}

void Algae::reproduce(Vector2 parentPosition, const Environment & environment, bool enlarge){
    switch(type){
        case AlgaeType::floating_algae:
            pos = parentPosition;
            velocity = angleToVector(randomBetween(0.0f, 2.0f * PI)) * 0.6;
            break;
        case AlgaeType::glass_algae:
            pos = parentPosition;
            break;
    }
    dormantTimer = 0;
    color = randomBetween(NORMAL_COLOR_RANGE);

    if(enlarge)
        radius = randomBetween(LARGE_SIZE_RANGE);
}

void Algae::resetReproduction(){
    timeToReproduce = randomBetween(REPRODUCTION_COOLDOWN);
}

void Algae::waitForReproduction(float waterLevel, const vector<Algae> & algaes,
    size_t itsIdx
){
    if(!active || waterLevel <= 0 || dormantTimer > 0 || timeToReproduce == 0)
        return;

    --timeToReproduce;
}

void Algae::updateLifespan(){
    if(lifespan > 0){
        if(dormantTimer <= 0)
            --lifespan;
        else
            lifespan -= AGING_SPEED_IN_DORMANT_STATE;
    }
    else
        alive = false;
}

void Algae::updateOutsideTheWater(){
    //Apply gravity force
    switch(type){
        case AlgaeType::floating_algae:
            velocity.y += GRAVITY * (radius / Algae::SIZE_RANGE.y);
            break;
        case AlgaeType::glass_algae:
            if(!attached)
                velocity.y += GRAVITY * (radius / Algae::SIZE_RANGE.y);
            break;
        default:
            break;
    }
    
    if(timeUntilGoingDormant > 0 && dormantTimer <= 0){
        --timeUntilGoingDormant;
        if(timeUntilGoingDormant <= 0){
            dormantTimer = randomBetween(DORMANT_STATE_DURATION_RANGE);
            color = randomBetween(DORMANT_COLOR_RANGE);
        }
    }
}

void Algae::updateInsideTheWater(const Environment & environment, const vector<Algae> & algaes,
    size_t itsIdx
){
    if(dormantTimer > 0){
        --dormantTimer;
        if(dormantTimer == 0)
            color = randomBetween(NORMAL_COLOR_RANGE);
    }

    timeUntilGoingDormant = TIME_UNTIL_GOING_DORMANT;

    //Apply gravity and buoyancy
    switch(type){
        case AlgaeType::floating_algae:
            velocity.y = - (pos.y - environment.getWaterSurfaceY()) * 0.01
                * (radius / Algae::SIZE_RANGE.y);
            break;
        case AlgaeType::glass_algae:
            if(!attached){
                velocity.y = - (pos.y - environment.getWaterSurfaceY()) * 0.01
                    * (radius / Algae::SIZE_RANGE.y);
                if(timeToAttach > 0)
                    --timeToAttach;
                else
                    attached = true;
            }
            break;
        default:
            break;
    }

    waitForReproduction(environment.waterLevel, algaes, itsIdx);
}

void Algae::update(const Environment & environment, const vector<Algae> & algaes, size_t itsIdx){
    if(!alive)
        return;

    updateLifespan();

    if(pos.y < environment.getWaterSurfaceY())
        updateOutsideTheWater();
    else
        updateInsideTheWater(environment, algaes, itsIdx);

    //Simple horizontal slowdown
    velocity.x = std::min(velocity.x * 0.9, 1.0);

    if(attached || detectCollisionWithAquarium(pos, environment))
        velocity = {0.0f, 0.0f};
    else
        pos = pos + velocity;
}