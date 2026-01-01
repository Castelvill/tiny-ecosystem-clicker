#include "entities.hpp"

class Simulation{
    Environment ecosystem;
    vector<Algae> algaes;
    vector<Sand> sand;
    vector<Ostracod> ostracods;
public:
    Simulation();

    void spawnAlgae();
    void updateAlgaes();

    void spawnSand();
    void updateSand();

    void spawnOstracods();
    void updateOstracods();

    void update();
    void draw() const;
};