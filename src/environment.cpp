#include "environment.hpp"

Environment::Environment(){
    position = {0, 0};
    size = {0, 0};
    waterLevel = 0.0f;
    maxWaterLevel = 0.0f;
}

float Environment::getWaterSurfaceY() const {
    if(size.y == 0)
        return MAXFLOAT;
    return std::min(size.y - waterLevel/size.x, size.y);
}