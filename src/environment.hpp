#pragma once
#include "utilities.hpp"

const int MIN_SCREEN_WIDTH = 440;
const int MIN_SCREEN_HEIGHT = 440;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float GRAVITY = 0.1f;
const float MIN_SAND_FALL = 0.5f;
const float SPEED_LIMIT = 1.0f;
const int SATURATION_FROM_ALGAE = 300;
const float NORMAL_WATER_DROPLET_VALUE = 100.0f;
const float WATER_DROPLET_VALUE_FACTOR = 0.001;

class Environment{
public:
    static constexpr Color WATER_COLOR = {40, 171, 255, 50};
    
    Vector2 position = {0, 0};
    Vector2 size = {0, 0};
    float waterLevel = 0.0f;
    float maxWaterLevel = 0.0f;

    Environment();
    float getWaterSurfaceY() const;
};