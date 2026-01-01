const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;
const int INITIAL_WATER_LEVEL = SCREEN_HEIGHT * 0.8;
const float GRAVITY = 0.1f;
const float MIN_SAND_FALL = 0.5f;
const float SPEED_LIMIT = 1.0f;
const int SATURATION_FROM_ALGAE = 300;

class Environment{
public:
    float waterSurfaceY;
    float waterLevel;
    Environment();
};