#include "substrate.hpp"

Substrate::Substrate(SubstrateType substrateType){
    type = substrateType;
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(Substrate::SIZE_RANGE.x, Substrate::SIZE_RANGE.y);
    switch(substrateType){
        case SubstrateType::soil:
            color = randomBetween(Substrate::SOIL_COLOR_RANGE);
            break;
        case SubstrateType::gravel:
            color = randomBetween(Substrate::GRAVEL_COLOR_RANGE);
            break;
        case SubstrateType::sand:
            color = randomBetween(Substrate::SAND_COLOR_RANGE);
            break;
        default:
            break;
    }
}

SimpleCollisionType Substrate::checkFallingCollision(const Entity & movingEntity,
    const vector<Substrate> & substrate
){
    if(movingEntity.velocity.y <= 0)
        return SimpleCollisionType::none;

    bool collisionOnTheLeft = false;
    bool collisionOnTheRight = false;
    for(const Entity & solid : substrate){
        //Ignore itself and other falling substrate particles
        if(movingEntity.pos == solid.pos || solid.active)
            continue;

        float distance = getDistance(movingEntity.pos, solid.pos);
        if(distance > movingEntity.radius + solid.radius)
            continue;

        if(solid.pos.x <= movingEntity.pos.x)
            collisionOnTheLeft = true;
        else
            collisionOnTheRight = true;

        if(collisionOnTheLeft && collisionOnTheRight)
            return SimpleCollisionType::full;
    }
    if(collisionOnTheLeft)
        return SimpleCollisionType::left;
    if(collisionOnTheRight)
        return SimpleCollisionType::right;
    return SimpleCollisionType::none;
}

void Substrate::stopSubstrate(){
    velocity = {0, 0};
    active = false;
}

void Substrate::update(Environment & environment, vector<Substrate> & substrate,
    size_t currentSandIdx
){
    if(!active)
        return;

    float waterSurfacePosY = environment.getWaterSurfaceY();

    //Gravity and buoyancy
    if(pos.y < waterSurfacePosY){
        velocity.y += GRAVITY * (radius / Substrate::SIZE_RANGE.y);
    }
    else{
        float buoyancy = 1.0f - (pos.y - waterSurfacePosY)
            / (environment.waterLevel / environment.size.x);
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }

    //When substrate particle hits the ground, stop it.
    if(detectCollisionWithAquarium(pos, environment)){
        active = false;
        return;
    }

    //Check simple collisions between falling (active) and settled substrate particles
    SimpleCollisionType collisionType = checkFallingCollision(*this, substrate);
    switch (collisionType){
        case SimpleCollisionType::full:
            stopSubstrate();
            return;
        case SimpleCollisionType::left:
            if(detectCollisionWithAquarium(pos + VEC2(1.0f, 0.0f), environment, true)){
                stopSubstrate();
                return;
            }
            pos.x += 1.0f;
            break;
        case SimpleCollisionType::right:
            if(detectCollisionWithAquarium(pos - VEC2(1.0f, 0.0f), environment, true)){
                stopSubstrate();
                return;
            }
            pos.x -= 1.0f;
            break;
        default:
            break;
    }

    pos.y += velocity.y;
}