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
    saturation = baseSaturation;
    radius = randomBetween(Ostracod::SIZE_RANGE.x, Ostracod::SIZE_RANGE.y);
    vision = randomBetween(Ostracod::VISION_RANGE.x, Ostracod::VISION_RANGE.y);
    reactionTime = randomBetween(Ostracod::REACTION_TIME_RANGE.x, Ostracod::REACTION_TIME_RANGE.y);
    speed = randomBetween(Ostracod::SPEED_RANGE.x, Ostracod::SPEED_RANGE.y);
}

Algae* Ostracod::findFirstVisibleAlgae(vector<Algae> & algaes){
    for(Algae & algea : algaes){
        float distance = getDistance(pos, algea.pos);
        if(distance <= vision)
            return &algea;
    }
    return nullptr;
}

void Ostracod::decideToMove(Environment & ecosystem, vector<Algae> & algaes, bool underwater){
    if(reactionCooldown > 0){
        reactionCooldown -= Ostracod::basicEnergyCost;
        return;
    }
        
    reactionCooldown = reactionTime;

    if(!underwater)
        speed *= (1.0f - Ostracod::outOfWaterSpeedPenality);

    Vector2 movement = VEC2(0.0f, 0.0f);
    Entity* visibleAlgae = nullptr;
    
    if(saturation <= Ostracod::baseSaturation)
        visibleAlgae = findFirstVisibleAlgae(algaes);

    if(visibleAlgae == nullptr){ //Move randomly
        movement = VEC2(
            randomBetween(-speed, speed),
            randomBetween(-speed, speed)
        );
    }
    else{ //Move to algae
        movement = VEC2(
            visibleAlgae->pos.x - pos.x,
            visibleAlgae->pos.y - pos.y
        );
        if(movement.x > 0)
            movement.x = std::min(movement.x, speed);
        if(movement.x < 0)
            movement.x = std::max(movement.x, -speed);
        if(movement.y > 0)
            movement.y = std::min(movement.y, speed);
        if(movement.y < 0)
            movement.y = std::max(movement.y, -speed);
    }

    velocity.x += movement.x;
    velocity.y += movement.y;
}

void Ostracod::update(Environment & ecosystem, vector<Algae> & algaes){
    bool underwater = pos.y >= ecosystem.waterSurfaceY;

    if(active && alive){
        saturation -= Ostracod::basicEnergyCost;
        if(saturation <= 0){
            alive = false;
            return;
        }
        decideToMove(ecosystem, algaes, underwater);
    }
    else{
        if(velocity.x > Ostracod::afterDeathSpeedPenalityPrecision
            || velocity.x < -Ostracod::afterDeathSpeedPenalityPrecision
        )
            velocity.x *= (1.0f - Ostracod::afterDeathSpeedPenality);
        else
            velocity.x = 0.0f;
    }

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

    if(velocity.x == 0.0f && velocity.y == 0.0f)
        return;

    //Limit velocity
    if(velocity.x > 0.0f)
        velocity.x = std::min(velocity.x, SPEED_LIMIT);
    if(velocity.x < 0.0f)
        velocity.x = std::max(velocity.x, -SPEED_LIMIT);
    if(velocity.y > 0.0f)
        velocity.y = std::min(velocity.y, SPEED_LIMIT);
    if(velocity.y < 0.0f)
        velocity.y = std::max(velocity.y, -SPEED_LIMIT);

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    //Stop ostracods from leaving the aquarium
    //Floor
    if(nextPosition.y + radius >= SCREEN_HEIGHT){
        velocity.y = 0.0f;
    }
    //Right wall
    if(nextPosition.x + radius >= SCREEN_WIDTH){
        velocity.x = 0.0f;
    }
    //Left wall
    if(nextPosition.x - radius <= 0){
        velocity.x = 0.0f;
    }

    //Eat one algae
    if(active && alive && saturation < Ostracod::maxSaturation){
        for(auto algae = algaes.begin(); algae != algaes.end();){
            float distance = getDistance(pos, algae->pos);
            if(distance <= radius + algae->radius){
                algae = algaes.erase(algae);
                saturation += SATURATION_FROM_ALGAE;
                break;
            }
            else{
                ++algae;
            }
        }
    }

    pos.x += velocity.x;
    pos.y += velocity.y;
}