#include "waterDroplet.hpp"

WaterDroplet::WaterDroplet(){
    active = true;
    pos = GetMousePosition();
    velocity = VEC2(0.0f, 0.0f);
    radius = randomBetween(WaterDroplet::SIZE_RANGE.x, WaterDroplet::SIZE_RANGE.y);
}

void WaterDroplet::update(Environment & environment){
    applyGravityAndBuoyancy(environment, false, radius / WaterDroplet::SIZE_RANGE.y);
    pos.y += velocity.y;
}