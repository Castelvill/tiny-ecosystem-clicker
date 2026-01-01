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

Sand::Sand(){
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(Sand::SIZE_RANGE.x, Sand::SIZE_RANGE.y);
}

void Sand::settle(){
    velocity.y = 0.0f;
    active = false;
}

void Sand::update(Environment & ecosystem, vector<Sand> & sand, size_t currentSandIdx){
    if(!active){
        return;
    }

    //Gravity and buoyancy
    if(pos.y < ecosystem.waterSurfaceY){
        velocity.y += GRAVITY * (radius / Sand::SIZE_RANGE.y);
    }
    else{
        float buoyancy = 1.0f - (pos.y - ecosystem.waterSurfaceY) / ecosystem.waterLevel;
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }

    //When sand particles hit the ground, stop them.
    if(pos.y + radius >= SCREEN_HEIGHT){
        settle();
        return;
    }

    //Check simple collisions between falling (active) and settled sand particles
    if(velocity.y > 0){
        bool collisionOnTheLeft = false;
        bool collisionOnTheRight = false;
        for(size_t other = 0; other < sand.size(); ++other){
            //Ignore itself and other falling sand particles
            if(other == currentSandIdx || sand[other].active)
                continue;

            float distance = getDistance(pos, sand[other].pos);
            if(distance > radius + sand[other].radius)
                continue;

            if(sand[other].pos.x <= pos.x)
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

void Ostracod::decideToMove(Environment & ecosystem, vector<Algae> & algaes, bool underwater){
    if(reactionCooldown > 0){
        reactionCooldown -= Ostracod::BASIC_ENERGY_COST;
        return;
    }

    float acceleration = speed;
        
    reactionCooldown = reactionTime;

    if(!underwater)
        acceleration *= (1.0f - Ostracod::OUTSIDE_WATER_SPEED_PENALITY);

    Vector2 movementVector = VEC2(0.0f, 0.0f);
    Entity* visibleAlgae = nullptr;
    
    if(saturation <= Ostracod::STARVING_TRIGGER)
        visibleAlgae = findNearestAlgae(algaes);

    if(visibleAlgae == nullptr){ //Move randomly
        float theta = randomBetween(0, 2 * PI);
        float radius = acceleration * std::sqrt(randomBetween(0, 1));

        movementVector = VEC2(
            radius * std::cos(theta),
            radius * std::sin(theta)
        );
    }
    else{ //Move to algaes
        movementVector = VEC2(
            visibleAlgae->pos.x - pos.x,
            visibleAlgae->pos.y - pos.y
        );
        limitVector(movementVector, acceleration);
    }

    velocity.x += movementVector.x;
    velocity.y += movementVector.y;
}

void Ostracod::update(Environment & ecosystem, vector<Algae> & algaes, vector<Sand> & sand,
    size_t &aliveOstracods
){
    if(!active)
        return;
    
    bool underwater = pos.y >= ecosystem.waterSurfaceY;

    //Behavior when alive or dead
    if(alive){
        ++aliveOstracods;
        saturation -= Ostracod::BASIC_ENERGY_COST;
        if(saturation <= 0)
            alive = false;
        else
            decideToMove(ecosystem, algaes, underwater);
    }
    //Deaccelerate on x axis if dead
    if(!alive){ 
        if(velocity.x > Ostracod::AFTER_DEATH_SPEED_PENALITY_PRECISION
            || velocity.x < -Ostracod::AFTER_DEATH_SPEED_PENALITY_PRECISION
        )
            velocity.x *= (1.0f - Ostracod::AFTER_DEATH_SPEED_PENALITY);
        else
            velocity.x = 0.0f;
    }

    limitVector(velocity, SPEED_LIMIT);

    //Gravity and buoyancy
    if(!underwater)
        velocity.y += GRAVITY * (radius / Ostracod::SIZE_RANGE.y);
    else{
        float buoyancy = 1.0f - (pos.y - ecosystem.waterSurfaceY) / ecosystem.waterLevel;
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }

    if(velocity.x == 0.0f && velocity.y == 0.0f){
        if(!alive)
            active = false;
        return;
    }

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    //Stop ostracods from leaving the aquarium
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

    //Collisions with sand
    for(const Sand & sandIt : sand){
        if(sandIt.active)
            continue;
        float distance = getDistance(nextPosition, sandIt.pos);
        if(distance <= radius + sandIt.radius){
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            break;
        }
    }

    //Eat one algae
    if(alive && saturation < Ostracod::MAX_SATURATION){
        if(eatingCooldown > 0)
            --eatingCooldown;
        else{
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
    }

    

    pos.x += velocity.x;
    pos.y += velocity.y;
}