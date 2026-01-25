#include "algae.hpp"

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
