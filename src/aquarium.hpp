#include "gui.hpp"

class Aquarium{
    Environment environment;
    vector<WaterDroplet> waterDroplets;
    vector<Algae> algaes;
    vector<Substrate> substrate;
    vector<Ostracod> ostracods;
    vector<Plant> plants;
public:
    Aquarium();
    Aquarium(Vector2 newPosition, Vector2 newSize);
    ~Aquarium();

    void spawnWaterDroplet();
    void updateWaterDroplet();

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
    void removeEntitiesOutsideTheScreen(float currentScreenHeight);
    void transferEntitiesToOtherAquariums(vector<Aquarium> & aquariums, PlayerState & player);
    void removeWater(vector<Aquarium> & aquariums);
    void updateGameArea(const UserInterface & gui, vector<Aquarium> & aquariums,
        PlayerState & player
    );
    void drawAquarium() const;
    void drawEntities() const;
    void draw() const;
};