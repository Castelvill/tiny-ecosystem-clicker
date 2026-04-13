#pragma once
#include "gui.hpp"
#include "waterDroplet.hpp"
#include "ostracod.hpp"
#include <cassert>

enum class Direction{left, up, right, down, none};

class Sector{
    const static size_t MAX_LOW_DETAIL_ALGAE_SIZE = 15;
public:    
    const static constexpr Vector2 SIZE = {50, 50}; 
    const static size_t MAX_ALGAE_SIZE = 50; 

    Vector2 position;
    size_t column = 0, row = 0;
    size_t algaeCount = 0;
    bool useLowDetail = false;
    vector<size_t> neighbourIndexes;

    Sector();
    Sector(Vector2 newPosition, size_t newColumn, size_t newRow);
    Rectangle toRectangle() const;
    bool canSpawnAlgae();
};

class SectorController{
public:
    const static size_t LOW_DETAIL_SECTOR_LIMIT = 120;

    vector<Sector> sectors;
    Vector2 size;
    size_t rows, columns;

    void updateCoverage(Vector2 containerSize);
    void switchToLowDetailAlgaeMode(Sector & sectorIt);
};

struct AlgaeSpawnInfo{
    bool canSpawn = false;
    Vector2 spawnPosition = {0, 0};
    size_t sectorIdx = 0;
    bool useLowDetail = false;
};

class Aquarium{
    Environment environment;
    /* Used to control the local density of entities inside the aquarium.
    Must be recreated after loading the save file.*/
    SectorController sectorController;

    vector<WaterDroplet> waterDroplets;
    vector<Algae> algaes;
    vector<Substrate> substrate;
    vector<Ostracod> ostracods;
    vector<Plant> plants;
    vector<WholePlantData> wholePlants;

    bool reproduceAlgae(size_t algaeIdx);
    AlgaeSpawnInfo findSpawnLocation(Sector & sectorIt, size_t sectorIdx);
public:
    Aquarium();
    Aquarium(Vector2 newPosition, Vector2 newSize);
    Aquarium(const Rectangle & newAquariumRect);
    ~Aquarium();
    
    Vector2 getPosition() const;
    Rectangle getRectangle() const;
    size_t getAlgaeSize() const;

    void setPosition(Vector2 newPosition);

    void spawnWaterDroplet();
    void updateWaterDroplet();

    void spawnAlgae();
    void removeDeadAlgae();
    void updateAlgaes();
    void updateAlgaesOutsideAquarium();

    void spawnSoil();
    void spawnSand();
    void spawnGravel();
    void updateSubstrate();

    void spawnOstracod();
    void updateOstracods();

    void spawnSeed();
    void spawnSeed(Vector2 position);
    void updatePlants();

    void update();
    void updateOutsideAquarium();
    void removeEntitiesOutsideTheScreen(float currentScreenHeight);
    void transferEntitiesToOtherAquariums(vector<Aquarium> & aquariums, PlayerState & player);
    void removeWater(vector<Aquarium> & aquariums);
    void updateGameArea(const UserInterface & gui, vector<Aquarium> & aquariums,
        PlayerState & player
    );
    void drawAquarium() const;
    void drawEntities() const;
    void draw(bool drawSectors) const;
    void expand(Direction direction, float length);

    bool isExpansionDisabled() const;

    void saveToFile(std::ofstream & file) const;
    void loadFromFile(std::ifstream & file);
};