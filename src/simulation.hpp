#include "entities.hpp"

class Simulation{
    Environment ecosystem;
    vector<Algae> algaes;
    vector<Sand> sand;
    vector<Ostracod> ostracods;

    size_t totalOstracods = 0;
    size_t aliveOstracods = 0;
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