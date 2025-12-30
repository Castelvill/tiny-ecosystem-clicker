#include "entities.hpp"

Ostracod::Ostracod(){
    active = true;
    alive = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    saturation = baseSaturation;
    radius = randomBetween(Ostracod::sizeRange.x, Ostracod::sizeRange.y);
    vision = randomBetween(Ostracod::visionRange.x, Ostracod::visionRange.y);
    reactionTime = randomBetween(Ostracod::reactionTimeRange.x, Ostracod::reactionTimeRange.y);
    speed = randomBetween(Ostracod::speedRange.x, Ostracod::speedRange.y);
}

void Ostracod::decideToMove(EcosystemLite & ecosystem, bool underwater){
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
        visibleAlgae = findFirstVisibleAlgae(ecosystem.algaes);

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

void Ostracod::update(EcosystemLite & ecosystem){
    bool underwater = pos.y >= ecosystem.waterSurfaceY;

    if(active && alive){
        saturation -= Ostracod::basicEnergyCost;
        if(saturation <= 0){
            alive = false;
            return;
        }
        decideToMove(ecosystem, underwater);
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
        velocity.y += gravity * (radius / maxParticleRadius);
    else{
        float buoyancy = 1.0f - (pos.y - ecosystem.waterSurfaceY) / ecosystem.waterLevel;
        buoyancy += minSandFall;

        if(velocity.y == 0.0f){
            velocity.y = buoyancy;
        }
        velocity.y = std::min(buoyancy, velocity.y);
    }

    if(velocity.x == 0.0f && velocity.y == 0.0f)
        return;

    //Limit velocity
    if(velocity.x > 0.0f)
        velocity.x = std::min(velocity.x, speedLimit);
    if(velocity.x < 0.0f)
        velocity.x = std::max(velocity.x, -speedLimit);
    if(velocity.y > 0.0f)
        velocity.y = std::min(velocity.y, speedLimit);
    if(velocity.y < 0.0f)
        velocity.y = std::max(velocity.y, -speedLimit);

    Vector2 nextPosition = VEC2(
        pos.x + velocity.x,
        pos.y + velocity.y
    );

    //Stop ostracods from leaving the aquarium
    //Floor
    if(nextPosition.y + radius >= screenHeight){
        velocity.y = 0.0f;
    }
    //Right wall
    if(nextPosition.x + radius >= screenWidth){
        velocity.x = 0.0f;
    }
    //Left wall
    if(nextPosition.x - radius <= 0){
        velocity.x = 0.0f;
    }

    //Eat one algae
    if(active && alive && saturation < Ostracod::maxSaturation){
        for(auto algae = ecosystem.algaes.begin(); algae != ecosystem.algaes.end();){
            float distance = getDistance(pos, algae->pos);
            if(distance <= radius + algae->radius){
                algae = ecosystem.algaes.erase(algae);
                saturation += saturationFromAlgae;
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

Entity* Ostracod::findFirstVisibleAlgae(vector<Entity> & algaes){
    for(Entity & algea : algaes){
        float distance = getDistance(pos, algea.pos);
        if(distance <= vision)
            return &algea;
    }
    return nullptr;
}