#include "utilities.hpp"

const int MIN_SCREEN_WIDTH = 440;
const int MIN_SCREEN_HEIGHT = 440;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const float GRAVITY = 0.1f;
const float MIN_SAND_FALL = 0.5f;
const float SPEED_LIMIT = 1.0f;
const int SATURATION_FROM_ALGAE = 300;

class Environment{
public:
    static constexpr Color WATER_COLOR = {0, 131, 251, 100};
    
    Vector2 position = {0, 0};
    Vector2 size = {0, 0};
    float waterLevel = 0;

    Environment();
    float getWaterSurfaceY() const;
};