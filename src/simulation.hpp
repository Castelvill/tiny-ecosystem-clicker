#include "plants.hpp"

const int SPAWN_ALGAE_BUTTON = KEY_ONE;
const int SPAWN_SOIL_BUTTON = KEY_TWO;
const int SPAWN_GRAVEL_BUTTON = KEY_THREE;
const int SPAWN_SAND_BUTTON = KEY_FOUR;
const int SPAWN_OSTRACODS_BUTTON = KEY_FIVE;
const int SPAWN_SEED_BUTTON = KEY_SIX;

class Simulation{
    Environment environment;
    vector<Algae> algaes;
    vector<Substrate> substrate;
    vector<Ostracod> ostracods;
    vector<Plant> plants;

    size_t totalOstracods = 0;
    size_t aliveOstracods = 0;
public:
    Simulation();

    void spawnAlgae();
    void updateAlgaes();

    void spawnSoil();

    void spawnSand();
    void spawnGravel();
    void updateSubstrate();

    void spawnOstracod();
    void updateOstracods();

    void spawnSeed();
    void updatePlants();

    void update();
    void draw() const;
};