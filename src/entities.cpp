#include "entities.hpp"

Algae::Algae(){
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(Algae::SIZE_RANGE.x, Algae::SIZE_RANGE.y);
}

void Algae::update(float waterSurfaceY){
    //Gravity and buoyancy
    if(pos.y < waterSurfaceY){
        velocity.y += GRAVITY * (radius / Algae::SIZE_RANGE.y);
    }
    else{
        velocity.y = - (pos.y - waterSurfaceY) * 0.01
            * (radius / Algae::SIZE_RANGE.y);
    }
    pos.y += velocity.y;
}

Substrate::Substrate(SubstrateType substrateType){
    type = substrateType;
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(Substrate::SIZE_RANGE.x, Substrate::SIZE_RANGE.y);
}

void Substrate::settle(){
    velocity.y = 0.0f;
    active = false;
}

void Substrate::update(Environment & environment, vector<Substrate> & substrate,
    size_t currentSandIdx
){
    if(!active){
        return;
    }

    //Gravity and buoyancy
    if(pos.y < environment.waterSurfaceY){
        velocity.y += GRAVITY * (radius / Substrate::SIZE_RANGE.y);
    }
    else{
        float buoyancy = 1.0f - (pos.y - environment.waterSurfaceY) / environment.waterLevel;
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }

    //When substrate particles hit the ground, stop them.
    if(pos.y + radius >= SCREEN_HEIGHT){
        settle();
        return;
    }

    //Check simple collisions between falling (active) and settled substrate particles
    if(velocity.y > 0){
        bool collisionOnTheLeft = false;
        bool collisionOnTheRight = false;
        for(size_t other = 0; other < substrate.size(); ++other){
            //Ignore itself and other falling substrate particles
            if(other == currentSandIdx || substrate[other].active)
                continue;

            float distance = getDistance(pos, substrate[other].pos);
            if(distance > radius + substrate[other].radius)
                continue;

            if(substrate[other].pos.x <= pos.x)
                collisionOnTheLeft = true;
            else
                collisionOnTheRight = true;

            if(collisionOnTheLeft && collisionOnTheRight)
                break;
        }

        if(collisionOnTheLeft && collisionOnTheRight){
            settle();
            return;
        }
        else if(collisionOnTheLeft)
            pos.x += 1.0f;
        else if(collisionOnTheRight)
            pos.x -= 1.0f;
    }

    pos.y += velocity.y;
}

Ostracod::Ostracod(){
    active = true;
    alive = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    saturation = BASE_SATURATION;
    radius = randomBetween(Ostracod::SIZE_RANGE.x, Ostracod::SIZE_RANGE.y);
    vision = randomBetween(Ostracod::VISION_RANGE.x, Ostracod::VISION_RANGE.y);
    reactionTime = randomBetween(Ostracod::REACTION_TIME_RANGE.x, Ostracod::REACTION_TIME_RANGE.y);
    speed = randomBetween(Ostracod::SPEED_RANGE.x, Ostracod::SPEED_RANGE.y);
    goToWaterSurface = false;
}

Algae* Ostracod::findNearestAlgae(vector<Algae> & algaes){
    float shortestDistance = SCREEN_WIDTH * SCREEN_HEIGHT;
    Algae* nearestAlgae = nullptr;

    for(Algae & algeaIt : algaes){
        float distance = getDistance(pos, algeaIt.pos);
        if(/*distance <= vision &&*/ distance < shortestDistance){
            shortestDistance = distance;
            nearestAlgae = &algeaIt;
        }
    }
    return nearestAlgae;
}

void accelerateInRandomDirection(Vector2 & movementVector, float acceleration){
    movementVector = angleToVector(randomBetween(0.0f, 2.0f * PI));
    movementVector.x *= acceleration;
    movementVector.y *= acceleration;
}

void Ostracod::thinkAboutTheNextMove(const Environment & environment, vector<Algae> & algaes,
    Entity*& visibleAlgae
){
    if(reactionCooldown > 0){
        reactionCooldown -= Ostracod::BASIC_ENERGY_COST;
        return;
    }
    reactionCooldown = reactionTime;

    //Move randomly until ostracod gets unstuck
    if(goInRandomDirection){
        if(velocity.x != 0.0f && velocity.y != 0.0f){
            goInRandomDirection = false;
            goToWaterSurface = true;
        }
        else
            return;
    }
    
    //Check if ostracod reached the water surface 
    if(goToWaterSurface){
        if(pos.y >= environment.waterSurfaceY)
            goToWaterSurface = false;
        else
            return;
    }
    
    //Decide if ostracod should move towards the nearest algae
    if(saturation <= Ostracod::STARVING_TRIGGER){
        searchForFood = true;
        visibleAlgae = findNearestAlgae(algaes);
    }
    else
        searchForFood = false;
}

void Ostracod::thinkAndMove(Environment & environment, vector<Algae> & algaes, bool isUnderwater){
    float acceleration = speed;

    if(!isUnderwater)
        acceleration *= (1.0f - Ostracod::OUTSIDE_WATER_SPEED_PENALITY);

    Vector2 movementVector = VEC2(0.0f, 0.0f);
    Entity* visibleAlgae = nullptr;

    thinkAboutTheNextMove(environment, algaes, visibleAlgae);

    //Move to algaes
    if(searchForFood && visibleAlgae != nullptr){ 
        movementVector = VEC2(
            visibleAlgae->pos.x - pos.x,
            visibleAlgae->pos.y - pos.y
        );
        limitVector(movementVector, acceleration);
    }
    //Move up
    else if(goToWaterSurface) 
        movementVector.y -= acceleration;
    //Move randomly
    else 
        accelerateInRandomDirection(movementVector, acceleration);

    velocity.x += movementVector.x;
    velocity.y += movementVector.y;
}

void Ostracod::move(Environment & environment, vector<Algae> & algaes, bool isUnderwater){
    if(alive)
        thinkAndMove(environment, algaes, isUnderwater);
    else{ 
        //Deaccelerate on x axis
        if(velocity.x > Ostracod::AFTER_DEATH_SPEED_PENALITY_PRECISION
            || velocity.x < -Ostracod::AFTER_DEATH_SPEED_PENALITY_PRECISION
        )
            velocity.x *= (1.0f - Ostracod::AFTER_DEATH_SPEED_PENALITY);
        else
            velocity.x = 0.0f;
    }

    limitVector(velocity, SPEED_LIMIT);
}

void Entity::detectCollisionWithAquarium(const Vector2 nextPosition){
    //Floor
    if(velocity.y > 0.0f && nextPosition.y + radius >= SCREEN_HEIGHT){
        velocity.y = 0.0f;
    }
    //Right wall
    if(velocity.x > 0.0f && nextPosition.x + radius >= SCREEN_WIDTH){
        velocity.x = 0.0f;
    }
    //Left wall
    if(velocity.x < 0.0f && nextPosition.x - radius <= 0){
        velocity.x = 0.0f;
    }
}

void Ostracod::detectCollisions(vector<Substrate> & substrate, const Vector2 nextPosition){
    //Collisions with substrate
    for(const Substrate & substrateIt : substrate){
        if(substrateIt.active)
            continue;

        float distance = getDistance(nextPosition, substrateIt.pos);
        if(distance <= radius + substrateIt.radius){
            velocity.x = 0.0f;
            velocity.y = 0.0f;

            //When there's a substrate obstacle between this ostracod and algae:
            //1. Move randomly to unstuck it.
            //2. If ostracod didn't get unstuck, repeat 1., otherwise go to the surface.
            //3. After reaching the surface come back to searching food.
            if(searchForFood)
                goInRandomDirection = true;

            break;
        }
    }
}

void Ostracod::eatAlgae(vector<Algae> & algaes){
    if(alive && saturation < Ostracod::MAX_SATURATION)
        return;

    if(eatingCooldown > 0){
        --eatingCooldown;
        return;
    }
        
    for(auto algaeIt = algaes.begin(); algaeIt != algaes.end();){
        float distance = getDistance(pos, algaeIt->pos);
        if(distance <= radius + algaeIt->radius){
            algaeIt = algaes.erase(algaeIt);
            saturation += SATURATION_FROM_ALGAE;
            eatingCooldown = BASIC_EATING_COOLDOWN;
            break;
        }
        else{
            ++algaeIt;
        }
    }
}

bool Entity::checkIfUnderwater(Environment & environment){
    return pos.y >= environment.waterSurfaceY;
}
void Entity::applyGravityAndBuoyancy(Environment & environment, bool isUnderwater, float mass){
    if(!isUnderwater)
        velocity.y += GRAVITY * mass;
    else{
        float buoyancy = 1.0f - (pos.y - environment.waterSurfaceY) / environment.waterLevel;
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }
}

void Ostracod::update(Environment & environment, vector<Algae> & algaes,
    vector<Substrate> & substrate, size_t & aliveOstracods
){
    if(!active)
        return;

    //Kill when starved to death
    if(alive){
        saturation -= Ostracod::BASIC_ENERGY_COST;
        if(saturation <= 0)
            alive = false;
        else
            ++aliveOstracods;
    }
    
    bool isUnderwater = checkIfUnderwater(environment);

    move(environment, algaes, isUnderwater);

    applyGravityAndBuoyancy(environment, isUnderwater, radius / Ostracod::SIZE_RANGE.y);

    //Deactivate dead ostracods when it's not moving to free some processing power
    if(velocity.x == 0.0f && velocity.y == 0.0f){
        if(!alive)
            active = false;
        return;
    }

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    detectCollisions(substrate, nextPosition);

    eatAlgae(algaes);

    pos.x += velocity.x;
    pos.y += velocity.y;
}