#include "entities.hpp"

bool Entity::detectCollisionWithAquarium(const Vector2 nextPosition,
    const Environment & environment, bool ignoreVelocity, bool infinitWalls
){
    if(environment.size.x == 0.0f || environment.size.y == 0.0f)
        return false;

    //Floor
    if((ignoreVelocity || velocity.y > 0.0f) && nextPosition.y + radius >= environment.size.y){
        velocity.y = 0.0f;
        return true;
    }
    //Right wall
    if((ignoreVelocity || velocity.x > 0.0f) && nextPosition.x + radius >= environment.size.x
        && (infinitWalls || nextPosition.y - radius > 0)
    ){
        velocity.x = 0.0f;
        return true;
    }
    //Left wall
    if((ignoreVelocity || velocity.x < 0.0f) && nextPosition.x - radius <= 0
        && (infinitWalls || nextPosition.y - radius > 0)
    ){
        velocity.x = 0.0f;
        return true;
    }
    return false;
}
bool Entity::checkIfUnderwater(Environment & environment){
    return pos.y >= environment.getWaterSurfaceY();
}
void Entity::applyGravityAndBuoyancy(Environment & environment, bool isUnderwater, float mass){
    if(!isUnderwater)
        velocity.y += GRAVITY * mass;
    else{
        float buoyancy = 1.0f - ((pos.y - environment.getWaterSurfaceY())
            / (environment.waterLevel / environment.size.x));
        buoyancy += MIN_SAND_FALL;

        if(velocity.y == 0.0f)
            velocity.y = buoyancy;

        velocity.y = std::min(buoyancy, velocity.y);
    }
}