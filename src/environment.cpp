#include "environment.hpp"

void Environment::updateWaterSurfaceY(){
    waterSurfaceY = SCREEN_HEIGHT - waterLevel;
}

Environment::Environment(){
    waterLevel = INITIAL_WATER_LEVEL;
    updateWaterSurfaceY();
}