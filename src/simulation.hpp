#include "gui.hpp"

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
    ~Simulation();

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

    void update(const UserInterface & gui);
    void draw() const;
};