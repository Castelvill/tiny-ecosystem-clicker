const int MIN_SCREEN_WIDTH = 440;
const int MIN_SCREEN_HEIGHT = 440;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int INITIAL_WATER_LEVEL = SCREEN_HEIGHT * 0.7;
const int MIN_WATER_LEVEL = 0;
const int MAX_WATER_LEVEL = SCREEN_HEIGHT * 0.8;
const float GRAVITY = 0.1f;
const float MIN_SAND_FALL = 0.5f;
const float SPEED_LIMIT = 1.0f;
const int SATURATION_FROM_ALGAE = 300;

class Environment{
public:
    float waterSurfaceY;
    float waterLevel;
    void updateWaterSurfaceY();
    Environment();
};