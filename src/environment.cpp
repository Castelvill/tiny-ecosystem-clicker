#include "environment.hpp"

Environment::Environment(){
    waterLevel = INITIAL_WATER_LEVEL;
    waterSurfaceY = SCREEN_HEIGHT - waterLevel;
}