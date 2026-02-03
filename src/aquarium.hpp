#pragma once
#include "gui.hpp"
#include "waterDroplet.hpp"
#include "ostracod.hpp"
#include <cassert>

enum class Direction{left, up, right, down, none};

class Aquarium{
    Environment environment;
    vector<WaterDroplet> waterDroplets;
    vector<Algae> algaes;
    vector<Substrate> substrate;
    vector<Ostracod> ostracods;
    vector<Plant> plants;
    vector<WholePlantData> wholePlants;
public:
    Aquarium();
    Aquarium(Vector2 newPosition, Vector2 newSize);
    Aquarium(const Rectangle & newAquariumRect);
    ~Aquarium();
    
    Vector2 getPosition() const;
    Rectangle getRectangle() const;

    void setPosition(Vector2 newPosition);

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
    void spawnSeed(Vector2 position);
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
    void expand(Direction direction, float length);

    bool isExpansionDisabled() const;

    void saveToFile(std::ofstream & file) const;
    void loadFromFile(std::ifstream & file);
};